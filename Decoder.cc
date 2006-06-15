#include "Decoder.hh"
#include "str.hh"

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
Decoder::message_result(bool guaranteed, bool only_new)
{
  TokenPassSearch &tp = t.tp_search();
  tp.get_path(hist_vec, guaranteed, only_new);
  
  for (int i = hist_vec.size() - 1; i >= 0; i--) {
    msg::Message message(msg::M_RECOG);
    message.append(t.word(hist_vec[i]->word_id));
    out_queue.queue.push_back(message);
    out_queue.flush();
  }
}

void
Decoder::run()
{
  out_queue.queue.push_back(msg::Message(msg::M_READY));
  out_queue.flush();

  std::vector<float> log_probs;
  std::vector<std::string> fields;
  int frame = 0;
  while (1) {

    if (in_queue.get_eof())
      break;

    if (verbose)
      fprintf(stderr, "decoder: waiting for frame %d\n", frame);

    in_queue.flush();
    if (in_queue.queue.empty())
      continue;

    msg::Message &message = in_queue.queue.front();

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
      
      {
        msg::Message message(msg::M_MESSAGE);
        message.append(str::fmt(256, "frame %d processed", frame));
        out_queue.queue.push_back(message);
        out_queue.flush();
      }

      assert(ret);
      frame++;
      assert(t.frame() == frame);

      message_result(true, false);
    }

    // "End of acoustics" message
    //

    else if (message.type() == msg::M_PROBS_END) {
      log_probs.clear();
      t.set_one_frame(frame, log_probs);
      bool ret = t.run();
      assert(!ret);

      message_result(false, false);

      t.reset(0);
      frame = 0;
      
      out_queue.queue.push_back(msg::Message(msg::M_READY));
      out_queue.flush();
    }

    in_queue.queue.pop_front();
  }
}
