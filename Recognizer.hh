#ifndef RECOGNIZER_HH
#define RECOGNIZER_HH

#include <pthread.h>
#include "FeatureGenerator.hh"
#include "HmmSet.hh"
#include "msg.hh"
#include "Process.hh"

class Recognizer {
public:

  struct {
    enum { OK, FINISHING, FINISHED } status;

    pthread_t t;
    int fd_pr;
    int fd_pw;
    int fd_tr;
    int fd_tw;
  } ac_thread;

  int verbosity;
  std::string dec_command;
  Process dec_proc;
  msg::InQueue dec_in_queue;
  msg::OutQueue dec_out_queue;

  FeatureGenerator gen;
  HmmSet hmms;
  bool finishing;

  msg::InQueue stdin_queue;
  msg::OutQueue stdout_queue;
  msg::InQueue ac_in_queue;
  msg::OutQueue ac_out_queue;

public:
  Recognizer();
  void run();

  void debug();

private:
  void create_ac_thread();
  void create_decoder_process();
  void process_stdin_queue();
  void process_ac_in_queue();
  void process_dec_in_queue();
};

#endif /* RECOGNIZER_HH */
