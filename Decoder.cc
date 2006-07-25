#include "Decoder.hh"
#include "str.hh"

Decoder::Decoder()
  : paused(false), last_guaranteed_history(NULL)
{
}

void
Decoder::init(conf::Config &config)
{

  // Initialize decoder
  //

  verbose = config["verbose"].specified;

  t.select_decoder(0);
  t.set_optional_short_silence(1);
  t.set_cross_word_triphones(1);
  
  if (verbose)
    fprintf(stderr, "decoder: reading phoneme model %s\n", 
	    config["ph"].get_c_str());
  t.hmm_read(config["ph"].get_c_str());

  if (config["dur"].specified) {
    if (verbose)
      fprintf(stderr, "decoder: reading duration model %s\n",
	      config["dur"].get_c_str());
    t.duration_read(config["dur"].get_c_str());
  }

  t.set_lm_lookahead(1);
  t.set_word_boundary("<w>");

  if (verbose)
    fprintf(stderr, "decoder: reading lexicon %s\n",
	    config["lexicon"].get_c_str());
  t.lex_read(config["lexicon"].get_c_str());
  t.set_sentence_boundary("<s>", "</s>");

  if (verbose)
    fprintf(stderr, "decoder: reading language model %s\n",
	    config["binlm"].get_c_str());
  t.ngram_read(config["binlm"].get_c_str(), 1);

  if (verbose)
    fprintf(stderr, "decoder: reading lookahead language model %s\n",
	    config["lookahead"].get_c_str());
  t.read_lookahead_ngram(config["lookahead"].get_c_str(), 1);
  t.prune_lm_lookahead_buffers(0, 4);

  t.set_prune_similar(3);
  t.set_token_limit(config["token-limit"].get_int());
  t.set_global_beam(config["beam"].get_float());
  t.set_lm_scale(config["lm-scale"].get_float());
  t.set_duration_scale(1);
  t.use_one_frame_acoustics();
  t.set_print_text_result(0);
  t.reset(0);

  in_queue.enable(0);
  out_queue.enable(1);
}

void
Decoder::message_result(bool send_all)
{
  TokenPassSearch &tp = t.tp_search();

  // Debugging guaranteed histories
//   if (last_guaranteed_history != NULL) {
//     fprintf(stderr, "last_guaranteed_history:");
//     TPLexPrefixTree::WordHistory *hist = last_guaranteed_history;
//     while (hist->word_id >= 0) {
//       fprintf(stderr, " %d %s 0x%p ref %d\n", 
//               hist->word_start_frame,
//               t.word(hist->word_id).c_str(), 
//               hist,
//               hist->get_num_references());
//       hist = hist->prev_word;
//     }
//     fprintf(stderr, "\n");
//   }

//   if (last_guaranteed_history != NULL)
//     tp.ensure_all_paths_contain_history(last_guaranteed_history);

  tp.get_path(hist_vec, true, send_all ? NULL : last_guaranteed_history);
  bool all_guaranteed = true;
  msg::Message message(msg::M_RECOG);

  for (int i = hist_vec.size() - 1; i >= 0; i--) {
    TPLexPrefixTree::WordHistory *hist = hist_vec[i];
    assert(hist->get_num_references() > 0);
    if (hist->prev_word->get_num_references() == 1) {
      if (all_guaranteed)
        last_guaranteed_history = hist;
    }
    else {
      if (all_guaranteed)
        message.append("* ");
      all_guaranteed = false;
    }

    message.append(str::fmt(256, "%d ", hist->word_start_frame) +
                   t.word(hist->word_id) + " ");
  }
  message.append(str::fmt(256, "%d", frame));
  out_queue.queue.push_back(message);
  out_queue.flush();
}

void
Decoder::reset()
{
  t.reset(0);
  frame = 0;
  paused = false;
  last_guaranteed_history = NULL;
}

void
Decoder::run()
{
  out_queue.queue.push_back(msg::Message(msg::M_READY));
  out_queue.flush();

  std::vector<float> log_probs;
  std::vector<std::string> fields;
  frame = 0;
  while (1) {

    if (in_queue.get_eof())
      break;

    if (verbose)
      fprintf(stderr, "decoder: waiting for frame %d\n", frame);

    in_queue.flush();
    if (in_queue.queue.empty())
      continue;

    msg::Message &message = in_queue.queue.front();
    if (paused) {
      if (message.type() == msg::M_DECODER_UNPAUSE) {
        paused = false;
        in_queue.queue.pop_front();
        continue;
      }
      else if (message.type() == msg::M_RESET)
        paused = false;
      else {
        continue;
      }
    }

    // Process new frame
    //

    if (message.type() == msg::M_PROBS) {
      int num_log_probs = message.data_length() / 4;
      log_probs.resize(num_log_probs);

      for (int i = 0; i < num_log_probs; i++)
	log_probs[i] = endian::get4<float>(message.data_ptr() + 4 * i);

      t.set_one_frame(frame, log_probs);
      if (verbose)
        fprintf(stderr, "decoder: processing frame %d\n", frame);
      bool ret = t.run();
      if (verbose)
        fprintf(stderr, "decoder: frame %d processed\n", frame);

      assert(ret);
      frame++;
      assert(t.frame() == frame);

      message_result(false);
    }

    // "End of acoustics" message
    //

    else if (message.type() == msg::M_PROBS_END) {

      if (verbose)
        fprintf(stderr, "decoder: got PROBS_END\n");

      // NOTE: the decoder seems to crash if audio ends right away, so
      // we avoid running the decoder with empty audio.
      if (frame > 0) {
        log_probs.clear();
        t.set_one_frame(frame, log_probs);
        bool ret = t.run();
        assert(!ret);

        message_result(false);
      }
      out_queue.queue.push_back(msg::Message(msg::M_RECOG_END));
      out_queue.flush();
    }

    else if (message.type() == msg::M_DECODER_SETTING) {
      std::vector<std::string> fields;
      std::string line(message.data_ptr(), message.data_length());
      
      str::split(&line, " \t", true, &fields);
      if (fields.size() < 1)
        fprintf(stderr, "decoder: empty DECODER_SETTING message\n");
      else {
        if (fields[0] == "beam") {
          if (fields.size() != 2)
            fprintf(stderr, "decoder: invalid beam setting message\n");
          bool ok = true;
          float beam = str::str2float(&fields[1], &ok);
          t.set_global_beam(beam);
          fprintf(stderr, "decoder: set beam to %g\n", beam);
        }

        else if (fields[0] == "lm_scale") {
          if (fields.size() != 2)
            fprintf(stderr, "decoder: invalid lm_scale setting message\n");
          bool ok = true;
          float lm_scale = str::str2float(&fields[1], &ok);
          t.set_lm_scale(lm_scale);
          fprintf(stderr, "decoder: set lm_scale to %g\n", lm_scale);
        }

      }
    }

    else if (message.type() == msg::M_RESET) {
      if (verbose)
        fprintf(stderr, "decoder: got RESET\n");
      reset();
      out_queue.queue.push_back(msg::Message(msg::M_READY));
      out_queue.flush();
    }

    else if (message.type() == msg::M_DECODER_PAUSE)
      paused = true;

    in_queue.queue.pop_front();
  }
}
