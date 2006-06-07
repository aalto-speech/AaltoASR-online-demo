#ifndef RECOGNIZER_HH
#define RECOGNIZER_HH

#include <pthread.h>
#include "FeatureGenerator.hh"
#include "msg.hh"

class Recognizer {
public:

  struct {
    enum { OK, FINISHING, FINISHED } status;

    pthread_t t;
    int fd_pr;
    int fd_pw;
    int fd_tr;
    int fd_tw;
    FILE *file;
  } ac_thread;

  FeatureGenerator gen;
  bool finish;

  msg::InQueue stdin_queue;
  msg::OutQueue stdout_queue;
  msg::InQueue ac_in_queue;
  msg::OutQueue ac_out_queue;

public:
  void create_ac_thread();
  void process_stdin_queue();
  void process_ac_in_queue();
  void run();
};

#endif /* RECOGNIZER_HH */
