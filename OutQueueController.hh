
#ifndef OUTQUEUECONTROLLER_HH_
#define OUTQUEUECONTROLLER_HH_

#include <pthread.h>
#include "msg.hh"
//#include "ErrorHandler.hh"

class OutQueueController  //:  public ErrorHandler
{
  
public:
  
  OutQueueController(int fd);
  ~OutQueueController();
  
//  bool start_flushing();
//  void stop_flushing();

  void flush();
  
  void send_message(const msg::Message &message);
  void reset();
  
private:

//  static void* flush(void *user_data);
  
  msg::OutQueue m_out_queue;
//  pthread_t m_thread;
  pthread_mutex_t m_lock;
//  bool m_stop;
  bool m_sending_message;
};

#endif /*OUTQUEUECONTROLLER_HH_*/
