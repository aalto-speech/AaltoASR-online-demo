
#ifndef QUEUECONTROLLER_HH_
#define QUEUECONTROLLER_HH_

#include <pthread.h>
//#include <list>
//#include "OutQueueController.hh"
#include "Recognition.hh"
#include "msg.hh"

class QueueController
{

public:

  QueueController(msg::InQueue *in_queue,
                  //OutQueueController *out_queue,
                  Recognition *recognition);

  ~QueueController();

  bool start();
  void stop();
  void enable();
  void disable();
  inline bool is_broken_pipe() const { return this->m_broken_pipe; }

/*
  inline void start() { this->m_stop = false; }
  inline void stop() { this->m_stop = true; }
  inline void enable(bool enabled) { this->m_enabled = enabled; }
//*/
//  static void* in_queue_listener(void *user_data);

private:

  static void* callback(void *user_data);
  void run();

  void parse_message(const std::string &message);

  Recognition *m_recognition;
  msg::InQueue *m_in_queue;
//  OutQueueController *m_out_queue;
  bool m_stop;
  bool m_enabled;
  bool m_thread_created; // just to prevent starting many threads..
  bool m_broken_pipe;
  pthread_t m_thread;
  pthread_mutex_t m_disable_lock;

//  pthread_t m_thread;
//  bool m_end_thread;
//  bool m_thread_created;
  
};

#endif /*QUEUECONTROLLER_HH_*/
