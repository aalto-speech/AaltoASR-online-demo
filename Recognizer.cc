#include <errno.h>
#include "Recognizer.hh"
#include "conf.hh"
#include "msg.hh"
#include "str.hh"

FeatureGenerator gen;

static void*
acoustic_thread(void *data)
{
  Recognizer *rec = (Recognizer*)data;

  if (rec->verbosity > 0)
    fprintf(stderr, "acoustic thread started\n");

  FILE *file = fdopen(rec->ac_thread.fd_tr, "r");
  if (file == NULL) {
    perror("acoustic_thread(): fdopen() failed");
    exit(1);
  }
  rec->gen.open(file, true);

  msg::OutQueue out_queue(rec->ac_thread.fd_tw);
  
  // Signal that we are ready
  {
    out_queue.queue.push_back(msg::Message(msg::M_READY));
    out_queue.flush();
  }

  int frame = 0;
  while (1) {
    assert(out_queue.empty());

    const FeatureVec vec = rec->gen.generate(frame);
    if (rec->gen.eof()) {
      if (close(rec->ac_thread.fd_tw) < 0) {
        perror("acoustic_thread: close() failed");
        exit(1);
      }
      break;
    }
    if (rec->verbosity > 0)
      fprintf(stderr, "acoustic_thread: generated frame %d\n", frame);

    // Send features
    //
//     {
//       size_t size = sizeof(float) * vec.dim();
//       msg::Message message(msg::M_FEATURES);
//       std::string buf(size, 0);
//       for (int i = 0; i < vec.dim(); i++)
// 	endian::put4(vec[i], &buf[i * 4]);
//       message.append(buf);

//       out_queue.queue.push_back(message);
//       out_queue.flush();
//     }

    // Send state probabilities
    //
    {
      rec->hmms.compute_observation_log_probs(vec);
      size_t size = sizeof(float) * rec->hmms.num_states();
      msg::Message message(msg::M_PROBS);
      std::string buf(size, 0);
      for (int i = 0; i < (int)rec->hmms.num_states(); i++)
        endian::put4(rec->hmms.obs_log_probs[i], &buf[i * 4]);
      message.append(buf);
      
      out_queue.queue.push_back(message);
      out_queue.flush();
    }

    frame++;
  }

  if (fclose(file) != 0) {
    perror("acoustic_thread(): fclose() failed");
    exit(1);
  }

  if (rec->verbosity > 0)
    fprintf(stderr, "acoustic thread finished\n");

  return NULL;
}

Recognizer::Recognizer()
  : verbosity(0), finishing(false)
{
}

void
Recognizer::create_ac_thread()
{
  int pipe1[2];
  int pipe2[2];
  int ret;

  ret = pipe(pipe1);
  assert(ret >= 0);
  ret = pipe(pipe2);
  assert(ret >= 0);

  ac_thread.fd_pr = pipe1[0];
  ac_thread.fd_tw = pipe1[1];

  ac_thread.fd_tr = pipe2[0];
  ac_thread.fd_pw = pipe2[1];
    
//  pthread_attr_t attr;
//  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
//  ret = pthread_create(&ac_thread.t, &attr, acoustic_thread, this);
  ret = pthread_create(&ac_thread.t, NULL, acoustic_thread, this);

  if (ret != 0) {
    fprintf(stderr, "pthread_create() failed with code %d\n", ret);
    exit(1);
  }
    
//  pthread_attr_destroy(&attr);

  msg::set_non_blocking(ac_thread.fd_pr);
  msg::set_non_blocking(ac_thread.fd_pw);

  ac_in_queue.enable(ac_thread.fd_pr);
  ac_out_queue.enable(ac_thread.fd_pw);

  ac_thread.status = ac_thread.OK;
}

void
Recognizer::create_decoder_process()
{
  if (dec_proc.create() == 0) {

    std::vector<std::string> fields;
    str::split(&dec_command, " \t", true, &fields);

    std::vector<char *> argv(fields.size() + 1);
    for (int i = 0; i < (int)fields.size(); i++)
      argv[i] = strdup(fields[i].c_str());
    argv.push_back(NULL);

    fprintf(stderr, "running decoder %s\n", dec_command.c_str());
    execv(argv[0], &argv[0]);
    perror("create_decoder_process(): exec() failed\n");
    exit(1);
  }

  msg::set_non_blocking(dec_proc.read_fd);
  msg::set_non_blocking(dec_proc.write_fd);

  dec_in_queue.enable(dec_proc.read_fd);
  dec_out_queue.enable(dec_proc.write_fd);
}

void
Recognizer::process_stdin_queue()
{
  while (!stdin_queue.empty()) {
    msg::Message &message = stdin_queue.queue.front();

    if (message.type() == msg::M_AUDIO_END)
    {
      if (ac_thread.status == ac_thread.OK) {
        ac_out_queue.disable();
        if (::close(ac_thread.fd_pw) < 0) {
          perror("process_stdin_queue(): close() failed");
	  exit(1);
	}
	ac_thread.status = ac_thread.FINISHING;
      }
    }

    else if (message.type() == msg::M_RESET)
    {
      if (verbosity > 0)
        fprintf(stderr, "rec: got reset from gui\n");
      ac_out_queue.disable();
      if (::close(ac_thread.fd_pw) < 0) {
        perror("process_stdin_queue(): close() failed");
        exit(1);
      }
      ac_thread.status = ac_thread.RESETTING;
      dec_out_queue.queue.clear();
      dec_out_queue.queue.push_back(msg::Message(msg::M_PROBS_END));
      dec_out_queue.flush();
    }

    else if (message.type() == msg::M_AUDIO)
    {
      if (verbosity > 0)
        fprintf(stderr, "rec: got audio from gui\n");
      // ac_thread wants raw audio data without header, because
      // FeatureGenerator reads it directly from FILE* 
      message.raw = true;
      ac_out_queue.queue.push_back(message);

      if (verbosity > 0)
        fprintf(stderr, "rec: sending audio to ac\n");
      ac_out_queue.flush();
    }

    stdin_queue.queue.pop_front();
  }
}

void
Recognizer::process_ac_in_queue()
{
  if (ac_in_queue.get_eof()) {

    if (ac_thread.status != ac_thread.RESETTING) {
      dec_out_queue.queue.push_back(msg::Message(msg::M_PROBS_END));
      dec_out_queue.flush();
    }

    int ret = pthread_join(ac_thread.t, NULL);
    if (ret != 0) {
      perror("process_ac_in_queue(): pthread_join failed");
      exit(1);
    }
    ::close(ac_thread.fd_pr);
    ac_in_queue.disable();

    if (finishing) {
      fprintf(stderr, "finished\n");
      exit(0);
    }

    create_ac_thread();
  }

  if (finishing) {
    ac_in_queue.queue.clear();
    return;
  }

  while (!ac_in_queue.empty()) {

    if (ac_thread.status == ac_thread.RESETTING) {
      ac_in_queue.queue.pop_front();
      continue;
    }

    msg::Message &message = ac_in_queue.queue.front();

    if (message.type() == msg::M_PROBS) {
      if (verbosity > 0) {
        fprintf(stderr, "rec: got PROBS from ac\n");
        fprintf(stderr, "rec: sending PROBS to dec\n");
      }
      dec_out_queue.queue.push_back(message);
      dec_out_queue.flush();
    }

    else if (message.type() == msg::M_FEATURES) {
      if (verbosity > 0)
        fprintf(stderr, "rec: got FEATURES from ac\n");
      stdout_queue.queue.push_back(message);
      stdout_queue.flush();
    }

    else if (message.type() == msg::M_READY) {
      if (verbosity > 0)
        fprintf(stderr, "rec: got READY from ac\n");
      ac_thread.status = ac_thread.OK;
    }

    ac_in_queue.queue.pop_front();
  }
}

void
Recognizer::process_dec_in_queue()
{
  if (dec_in_queue.get_eof()) {
    fprintf(stderr, "rec: eof from decoder process\n");
    exit(1);
  }

  if (finishing) {
    dec_in_queue.queue.clear();
    return;
  }

  while (!dec_in_queue.empty()) {
    msg::Message &message = dec_in_queue.queue.front();

    if (message.type() == msg::M_RECOG) {
      if (verbosity > 0) {
        fprintf(stderr, "rec: got RECOG from dec\n");
        fprintf(stderr, "rec: sending RECOG to gui\n");
      }
      stdout_queue.queue.push_back(message);
      stdout_queue.flush();
    }

    else if (message.type() == msg::M_MESSAGE) {
      stdout_queue.queue.push_back(message);
      stdout_queue.flush();
    }

    else if (message.type() == msg::M_READY) {
      if (verbosity > 0) {
        fprintf(stderr, "rec: got READY from dec\n");
        fprintf(stderr, "rec: sending READY to gui\n");
      }
      stdout_queue.queue.push_back(message);
      stdout_queue.flush();
    }

    dec_in_queue.queue.pop_front();
  }
}

void
Recognizer::run()
{
  stdin_queue.enable(0);
  stdout_queue.enable(1);

  if (hmms.dim() != gen.dim()) {
    msg::Message message(msg::M_ERROR);
    message.append(
      str::fmt(256, "HMM dimension %d differs from feature dimension %d",
	       hmms.dim(), gen.dim()));
    stdout_queue.queue.push_back(message);
    stdout_queue.flush();
    exit(1);
  }

  create_decoder_process();

  msg::set_non_blocking(0);
  msg::set_non_blocking(1);

  msg::Mux mux;
  mux.in_queues.push_back(&stdin_queue);
  mux.in_queues.push_back(&ac_in_queue);
  mux.in_queues.push_back(&dec_in_queue);
  mux.out_queues.push_back(&stdout_queue);
  mux.out_queues.push_back(&ac_out_queue);
  mux.out_queues.push_back(&dec_out_queue);

  create_ac_thread();

  finishing = false;
  while (1) {

    assert(stdin_queue.empty());
    assert(ac_in_queue.empty());
    assert(dec_in_queue.empty());

    if (stdin_queue.get_eof()) {
      fprintf(stderr, "rec: eof in input\n");
      stdin_queue.disable();
      stdout_queue.disable();
      ac_out_queue.disable();
      ::close(ac_thread.fd_pw);
      finishing = true;
    }

    mux.wait_and_flush();

    process_stdin_queue();
    process_ac_in_queue();
    process_dec_in_queue();
  };
}

template <class T>
void
debug_print_queue(const std::string &label, T &queue)
{
  fprintf(stderr, "%s\n", label.c_str());
  fprintf(stderr, "  size: %d\n", (int)queue.size());
  for (typename T::iterator it = queue.begin(); it != queue.end(); it++)
  {
    msg::Message &message = (*it);
    fprintf(stderr, "  - type %d, length %d\n", message.type(), 
            message.data_length());
  }
}

void
Recognizer::debug()
{
  debug_print_queue("stdin", stdin_queue.queue);
  debug_print_queue("stdout", stdout_queue.queue);
  debug_print_queue("ac_in", ac_in_queue.queue);
  debug_print_queue("ac_out", ac_out_queue.queue);
  debug_print_queue("dec_in", dec_in_queue.queue);
  debug_print_queue("dec_out", dec_out_queue.queue);
}
