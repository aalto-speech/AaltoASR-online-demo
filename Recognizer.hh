#ifndef RECOGNIZER_HH
#define RECOGNIZER_HH

#include <pthread.h>
#include "FeatureGenerator.hh"
#include "HmmSet.hh"
#include "msg.hh"
#include "Process.hh"

class Recognizer {
public:

  // If you change these, remember to change ac_state_str in Recognizer.cc
  enum AcState { A_STARTING, 
                 A_READY, 
                 A_EOA_PENDING, 
                 A_CLOSING, 
                 A_CLOSED, 
                 A_NULL };

  // If you change these, remember to change dec_state_str in Recognizer.cc
  enum DecState { D_STARTING, 
                  D_READY, 
                  D_EOP_PENDING, 
                  D_STALLED, 
                  D_CLOSED, 
                  D_NULL };
  
  AcState ac_state;
  DecState dec_state;
  bool quit_pending;

  struct {
    pthread_t t;
    int fd_pr;
    int fd_pw;
    int fd_tr;
    int fd_tw;
    pthread_mutex_t lock;
    bool reset_flag;
  } ac_thread;

  int verbosity;
  std::string dec_command;
  Process dec_proc;
  msg::InQueue dec_in_queue;
  msg::OutQueue dec_out_queue;

  FeatureGenerator gen;
  HmmSet hmms;

  msg::InQueue stdin_queue;
  msg::OutQueue stdout_queue;
  msg::InQueue ac_in_queue;
  msg::OutQueue ac_out_queue;

public:
  Recognizer();
  void run();

private:
  void change_state(AcState a, DecState d);
  void create_ac_thread();
  void create_decoder_process();
  void process_stdin_queue();
  void process_ac_in_queue();
  void process_dec_in_queue();
};

#endif /* RECOGNIZER_HH */
