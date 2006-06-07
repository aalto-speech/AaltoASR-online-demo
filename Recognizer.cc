#include <errno.h>
#include "Recognizer.hh"
#include "conf.hh"
#include "msg.hh"

FeatureGenerator gen;
int frame;

static void*
acoustic_thread(void *data)
{
  fprintf(stderr, "acoustic thread started\n");

  Recognizer *rec = (Recognizer*)data;

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

  while (1) {
    assert(out_queue.empty());

    const FeatureVec vec = rec->gen.generate(frame);
    fprintf(stderr, "acoustic_thread: generated frame %d\n", frame);
    if (rec->gen.eof()) {
      close(rec->ac_thread.fd_tw);
      break;
    }
    size_t size = sizeof(float) * vec.dim();

    msg::Message message(msg::M_FEATURES);

    std::string buf(size, 0);
    for (int i = 0; i < vec.dim(); i++)
      endian::put4(vec[i], &buf[i * 4]);
    message.append(buf);

    out_queue.queue.push_back(message);
    out_queue.flush();
    sleep(1);
    frame++;
  }

  if (fclose(file) != 0) {
    perror("acoustic_thread(): fclose() failed");
    exit(1);
  }

  fprintf(stderr, "acoustic thread finished\n");

  return NULL;
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
Recognizer::process_stdin_queue()
{
  while (!stdin_queue.empty()) {
    msg::Message &message = stdin_queue.queue.front();

    if (message.type() == msg::M_RESET)
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

    else if (message.type() == msg::M_AUDIO)
    {
      // ac_thread wants raw audio data without header, because
      // FeatureGenerator reads it directly from FILE* 
      message.raw = true;
      ac_out_queue.queue.push_back(message);
      ac_out_queue.flush();
    }

    stdin_queue.queue.pop_front();
  }
}

void
Recognizer::process_ac_in_queue()
{
  if (ac_in_queue.eof) {
    int ret = pthread_join(ac_thread.t, NULL);
    if (ret != 0) {
      perror("process_ac_in_queue(): pthread_join failed");
      exit(1);
    }
    fclose(ac_thread.file);
    ac_thread.file = NULL;
    create_ac_thread();
    
  }

  while (!ac_in_queue.empty()) {
    msg::Message &message = ac_in_queue.queue.front();

    if (message.type() == msg::M_PROBS) {
      fprintf(stderr, "rec: got PROBS from ac\n");
    }

    if (message.type() == msg::M_FEATURES) {
      fprintf(stderr, "rec: got FEATURES from ac\n");
      stdout_queue.queue.push_back(message);
      stdout_queue.flush();
    }

    else if (message.type() == msg::M_READY) {
      fprintf(stderr, "rec: got READY from ac\n");
      stdout_queue.queue.push_back(msg::Message(msg::M_READY));
      stdout_queue.flush();
    }

    ac_in_queue.queue.pop_front();
  }
}

void
Recognizer::run()
{
  msg::set_non_blocking(0);
  msg::set_non_blocking(1);

  stdin_queue.enable(0);
  stdout_queue.enable(1);

  msg::Mux mux;
  mux.in_queues.push_back(&stdin_queue);
  mux.in_queues.push_back(&ac_in_queue);
  mux.out_queues.push_back(&stdout_queue);
  mux.out_queues.push_back(&ac_out_queue);

  create_ac_thread();

  finish = false;
  while (!finish) {

    assert(stdin_queue.empty());
    assert(ac_in_queue.empty());

    if (stdin_queue.eof) {
      fprintf(stderr, "rec: eof in input, exiting\n");
      exit(0);
    }

    mux.wait_and_flush();
    fprintf(stderr, "rec: mux.wait_and_flush() returned\n");

    process_stdin_queue();
    process_ac_in_queue();
  };
}
