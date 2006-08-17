#include <errno.h>
#include "Recognizer.hh"
#include "conf.hh"
#include "msg.hh"
#include "str.hh"
#include "Adapter.hh"

static const char *ac_state_str[] = {
  "A_STARTING",
  "A_READY",
  "A_EOA_PENDING",
  "A_CLOSING",
  "A_CLOSED",
  "A_NULL"
};

static const char *dec_state_str[] = {
  "D_STARTING", 
  "D_READY", 
  "D_EOP_PENDING", 
  "D_STALLED", 
  "D_CLOSED", 
  "D_NULL" 
};

FeatureGenerator gen;

static void*
acoustic_thread(void *data)
{
  Recognizer *rec = (Recognizer*)data;

  if (rec->verbosity > 0)
    fprintf(stderr, "acoustic thread started\n");

  FILE *file = fdopen(rec->ac_thread.fd_tr, "r");
  if (file == NULL) {
    perror("ERROR: acoustic_thread(): fdopen() failed");
    exit(1);
  }
  rec->gen.open(file, true, true);

  msg::OutQueue out_queue(rec->ac_thread.fd_tw);
  
  // Signal that we are ready
  {
    out_queue.queue.push_back(msg::Message(msg::M_READY));
    out_queue.flush();
  }

  int frame = 0;
  while (1) {
    assert(out_queue.empty());

    if (rec->verbosity > 0)
      fprintf(stderr, "acoustic_thread: waiting for frame %d\n", frame);
    const FeatureVec vec = rec->gen.generate(frame);

    // Check if recognizer has raised the reset flag
    //
    bool got_reset = false;
    pthread_mutex_lock(&rec->ac_thread.lock);

    if (frame == 0)
      rec->feature_vectors.clear();
    rec->feature_vectors.push_back(std::vector<float>());
    vec.get(rec->feature_vectors.back());

    if (rec->ac_thread.reset_flag) {
      fprintf(stderr, "acoustic_thread: got RESET in frame %d\n", frame);
      got_reset = true;
      rec->ac_thread.reset_flag = false;
    }

    pthread_mutex_unlock(&rec->ac_thread.lock);

    if (got_reset || rec->gen.eof()) {
      int ret = close(rec->ac_thread.fd_tw);
      if (ret < 0) {
        perror("ERROR: acoustic_thread: close() failed");
        exit(1);
      }
      break;
    }

    // Compute state probabilities and send them to recognizer
    //
    if (rec->verbosity > 0)
      fprintf(stderr, "acoustic_thread: generated frame %d\n", frame);
    rec->hmms.compute_observation_log_probs(vec);
    size_t size = sizeof(float) * rec->hmms.num_states();
    msg::Message message(msg::M_PROBS);
    std::string buf(size, 0);
    for (int i = 0; i < (int)rec->hmms.num_states(); i++)
      endian::put4(rec->hmms.obs_log_probs[i], &buf[i * 4]);
    message.append(buf);
      
    out_queue.queue.push_back(message);
    out_queue.flush();

    frame++;
  }

  if (fclose(file) != 0) {
    perror("ERROR: acoustic_thread(): fclose() failed");
    exit(1);
  }

  if (rec->verbosity > 0)
    fprintf(stderr, "acoustic thread finished\n");

  return NULL;
}

Recognizer::Recognizer()
  : quit_pending(false), verbosity(0)
{
  ac_state = A_CLOSED;
  dec_state = D_CLOSED;
  adaptation = false;
}

void // private
Recognizer::change_state(AcState a, DecState d)
{
  if (verbosity > 0)
    fprintf(stderr, "(%s, %s) -> ", 
            ac_state_str[ac_state], dec_state_str[dec_state]);
  if (a != A_NULL)
    ac_state = a;
  if (d != D_NULL)
    dec_state = d;
  if (verbosity > 0)
    fprintf(stderr, "(%s, %s)\n", 
            ac_state_str[ac_state], dec_state_str[dec_state]);
}

void
Recognizer::create_ac_thread()
{
  assert(ac_state == A_CLOSED || 
         (ac_state == A_CLOSING && dec_state == D_STALLED));

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
    fprintf(stderr, "ERROR: pthread_create() failed with code %d\n", ret);
    exit(1);
  }
    
//  pthread_attr_destroy(&attr);

  msg::set_non_blocking(ac_thread.fd_pr);
  msg::set_non_blocking(ac_thread.fd_pw);

  ac_in_queue.enable(ac_thread.fd_pr);
  ac_out_queue.enable(ac_thread.fd_pw);
}

void
Recognizer::create_decoder_process()
{
  assert(dec_state == D_CLOSED);

  change_state(A_NULL, D_STARTING);
  if (dec_proc.create() == 0) {

    std::vector<std::string> fields;
    str::split(&dec_command, " \t", true, &fields);

    std::vector<char *> argv(fields.size() + 1);
    for (int i = 0; i < (int)fields.size(); i++)
      argv[i] = strdup(fields[i].c_str());
    argv.push_back(NULL);

    fprintf(stderr, "running decoder %s\n", dec_command.c_str());
    execv(argv[0], &argv[0]);
    perror("ERROR: create_decoder_process(): exec() failed\n");
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

    if (message.type() == msg::M_AUDIO_END) {
      if (ac_state == A_READY && dec_state == D_READY)
        change_state(A_EOA_PENDING, D_READY);
      else {
        fprintf(stderr, 
                "rec: ignoring AUDIO_END in ac_state %d dec_state %d\n",
                ac_state, dec_state);
      }
    }

    else if (message.type() == msg::M_AUDIO)
    {
      if (verbosity > 0)
        fprintf(stderr, "rec: got AUDIO from gui\n");

      if (ac_state == A_READY && dec_state == D_READY) {
        // ac_thread wants raw audio data without header, because
        // FeatureGenerator reads it directly from FILE* 
        message.raw = true;
        ac_out_queue.queue.push_back(message);
        ac_out_queue.flush();

        if (verbosity > 0)
          fprintf(stderr, "rec: sending audio to ac (len %d)\n", 
                  message.data_length());
      }
      else {
        fprintf(stderr, "rec: ignoring AUDIO in ac_state %d dec_state %d\n",
                ac_state, dec_state);
      }
    }

    else if (message.type() == msg::M_RESET)
    {
      if (verbosity > 0)
        fprintf(stderr, "rec: got RESET from gui\n");

      if (ac_state == A_CLOSED && dec_state == D_EOP_PENDING)
      {
        create_ac_thread();
        dec_out_queue.queue.clear();
        dec_out_queue.queue.push_back(message);
        dec_out_queue.flush();
        change_state(A_STARTING, D_STARTING);
      }
      else if (ac_state == A_CLOSING && dec_state == D_READY) {
        change_state(A_CLOSING, D_STALLED);
      }
      else if ((ac_state == A_EOA_PENDING && dec_state == D_READY) ||
               (ac_state == A_READY && dec_state == D_READY))
      {
        pthread_mutex_lock(&ac_thread.lock);
        ac_thread.reset_flag = true;
        pthread_mutex_unlock(&ac_thread.lock);
        ac_out_queue.queue.clear();
        ac_out_queue.disable();
        if (::close(ac_thread.fd_pw) < 0) {
          perror("ERROR: process_stdin_queue(): close() failed");
          exit(1);
        }
        change_state(A_CLOSING, D_STALLED);
      }
      else {
        fprintf(stderr, "rec: WARNING: ignoring RESET in "
                "ac_state %d dec_state %d which should not happen!\n",
                ac_state, dec_state);
      }
      stdin_queue.mux_suspend();
    }

    else if (message.type() == msg::M_ADAPT_ON) {
      adaptation = true;
      dec_out_queue.queue.push_back(message);
    }

    else if (message.type() == msg::M_ADAPT_OFF) {
      adaptation = false;
      dec_out_queue.queue.push_back(message);
    }

    else if (message.type() == msg::M_ADAPT_RESET) {
      Adapter adapter(hmms, gen);
      LinTransformModule *mllr_mod = 
        dynamic_cast<LinTransformModule*>(gen.module("mllr"));
      MllrTrainer::restore_identity(mllr_mod);
      fprintf(stderr, "rec: adaptation reset\n");
    }

    else if (message.type() == msg::M_DECODER_SETTING ||
             message.type() == msg::M_DECODER_PAUSE ||
             message.type() == msg::M_DECODER_UNPAUSE)
    {
      dec_out_queue.queue.push_back(message);
    }

    else if (message.type() == msg::M_DEBUG) {
      std::string str = message.data_str();
      if (str == "conf") {
        gen.write_configuration(stderr);
      }
    }

    stdin_queue.queue.pop_front();
  }
}

void
Recognizer::process_ac_in_queue()
{
  if (ac_in_queue.get_eof()) {

    int ret = pthread_join(ac_thread.t, NULL);
    if (ret != 0) {
      perror("ERROR: process_ac_in_queue(): pthread_join failed");
      exit(1);
    }
    ::close(ac_thread.fd_pr);
    ac_in_queue.disable();

    if (ac_state == A_CLOSING && dec_state == D_READY) {
      change_state(A_CLOSED, D_EOP_PENDING);
      dec_out_queue.queue.push_back(msg::Message(msg::M_PROBS_END));
      dec_out_queue.flush();
    }
    else if (ac_state == A_CLOSING && dec_state == D_STALLED) {
      create_ac_thread();
      change_state(A_STARTING, D_STARTING);
      dec_out_queue.queue.push_back(msg::Message(msg::M_RESET, true));
      dec_out_queue.flush();
    }
    else {
      fprintf(stderr, "ERROR: rec: unexpected eof from ac in "
              "ac_state = %d dec_state = %d\n", ac_state, dec_state);
      exit(1);
    }

    if (quit_pending) {
      fprintf(stderr, "finished\n");
      exit(0);
    }
  }

  if (quit_pending) {
    ac_in_queue.queue.clear();
    return;
  }

  while (!ac_in_queue.empty()) {

    msg::Message &message = ac_in_queue.queue.front();

    if (message.type() == msg::M_PROBS) {

      if ((ac_state == A_READY && dec_state == D_READY) ||
          (ac_state == A_EOA_PENDING && dec_state == D_READY) ||
          (ac_state == A_CLOSING && dec_state == D_READY))
      {
        if (verbosity > 0) {
          fprintf(stderr, "rec: got PROBS from ac\n");
          fprintf(stderr, "rec: sending PROBS to dec\n");
        }
        dec_out_queue.queue.push_back(message);
        dec_out_queue.flush();
      }
      else {
        fprintf(stderr, "rec: ignoring AUDIO in ac_state %d dec_state %d\n", 
                ac_state, dec_state);
      }
    }

    else if (message.type() == msg::M_READY) {
      if ((ac_state == A_STARTING && dec_state == D_STARTING) ||
          (ac_state == A_STARTING && dec_state == D_READY)) 
      {
        if (verbosity > 0)
          fprintf(stderr, "rec: got READY from ac\n");
        if (dec_state == D_READY) {
          stdout_queue.queue.push_back(message);
          stdout_queue.flush();
          stdin_queue.mux_release();
        }
        change_state(A_READY, D_NULL);
      }
      else {
        fprintf(stderr, 
                "ERROR: rec: got READY from ac in ac_state %d dec_state %d\n",
                ac_state, dec_state);
        exit(1);
      }
    }

    ac_in_queue.queue.pop_front();
  }
}

void
Recognizer::process_dec_in_queue()
{
  if (dec_in_queue.get_eof()) {
    fprintf(stderr, "ERROR: rec: eof from decoder process\n");
    exit(1);
  }

  if (quit_pending) {
    dec_in_queue.queue.clear();
    return;
  }

  while (!dec_in_queue.empty()) {
    msg::Message &message = dec_in_queue.queue.front();

    if (message.type() == msg::M_RECOG) {
      if ((ac_state == A_READY && dec_state == D_READY) ||
          (ac_state == A_EOA_PENDING && dec_state == D_READY) ||
          (ac_state == A_CLOSING && dec_state == D_READY) ||
          (ac_state == A_CLOSED && dec_state == D_EOP_PENDING))
      {
        stdout_queue.queue.push_back(message);
        stdout_queue.flush();
      }
      else {
        fprintf(stderr, "rec: ignoring RECOG in ac_state %d dec_state %d\n",
                ac_state, dec_state);
      }
    }

    else if (message.type() == msg::M_RECOG_END) {
      if (ac_state != A_CLOSED && dec_state != D_EOP_PENDING) {
        fprintf(stderr, "rec: ignoring RECOG_END from decoder in "
                "ac_state %d dec_state %d\n", ac_state, dec_state);
        exit(1);
      }
      stdout_queue.queue.push_back(message);
      stdout_queue.flush();
      create_ac_thread();
      change_state(A_STARTING, D_STARTING);
      dec_out_queue.queue.push_back(msg::Message(msg::M_RESET, true));
      dec_out_queue.flush();
    }

    else if (message.type() == msg::M_MESSAGE) {
      stdout_queue.queue.push_back(message);
      stdout_queue.flush();
    }

    else if (message.type() == msg::M_STATE_HISTORY) {
      pthread_mutex_lock(&ac_thread.lock);

      if (!adaptation) {
        fprintf(stderr, "rec: ignoring STATE_HISTORY when adaptation off\n");
      }

      Adapter adapter(hmms, gen);
      LinTransformModule *mllr_mod = 
        dynamic_cast<LinTransformModule*>(gen.module("mllr"));

      fprintf(stderr, "rec: adapting\n");
      adapter.adapt(message.data_str(), feature_vectors, mllr_mod);
      fprintf(stderr, "rec: adapting finished\n");

      pthread_mutex_unlock(&ac_thread.lock);
    }

    else if (message.type() == msg::M_READY) {
      if ((ac_state == A_STARTING && dec_state == D_STARTING) ||
          (ac_state == A_READY && dec_state == D_STARTING))
      {
        if (ac_state == A_READY) {
          stdout_queue.queue.push_back(message);
          stdout_queue.flush();
          stdin_queue.mux_release();
        }
        change_state(A_NULL, D_READY);
      }
      else {
        fprintf(stderr, "ERROR: rec: got READY from decoder in "
                "ac_state %d dec_state %d\n", ac_state, dec_state);
        exit(1);
      }
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
    msg::Message message(msg::M_MESSAGE);
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
  change_state(A_STARTING, D_NULL);
  quit_pending = false;
  while (1) {

    assert(stdin_queue.empty());
    assert(ac_in_queue.empty());
    assert(dec_in_queue.empty());

    if (stdin_queue.get_eof()) {
      fprintf(stderr, "rec: eof in input\n");
      stdin_queue.disable();
      ac_out_queue.disable();
      ::close(ac_thread.fd_pw);
      quit_pending = true;
      change_state(A_CLOSING, D_STALLED);
    }

    if (ac_state == A_EOA_PENDING && ac_out_queue.queue.empty()) {
      assert(dec_state == D_READY);
      change_state(A_CLOSING, D_NULL);
      ac_out_queue.disable();
      if (::close(ac_thread.fd_pw) < 0) {
        perror("ERROR: process_stdin_queue(): close() failed");
        exit(1);
      }
    }

    mux.wait_and_flush();

    process_ac_in_queue();
    process_dec_in_queue();

    // FIXME: currently stdin_queue must be processed after other
    // queues, because ac or dec can release suspended stddin_queue.
    process_stdin_queue();
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

