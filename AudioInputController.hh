
#ifndef AUDIOINPUTCONTROLLER_HH_
#define AUDIOINPUTCONTROLLER_HH_

#include <iostream>
#include <string.h>
#include <queue>
#include "OutQueueController.hh"
#include "AudioInputStream.hh"
//#include "ErrorHandler.hh"
//#include "FileOutput.hh"

class AudioInputController//  :  public ErrorHandler
{
  
public:
  
  AudioInputController(OutQueueController *out_queue);
  virtual ~AudioInputController();
  
  virtual bool initialize();
  virtual void terminate();

  // Starts a new thread which listens audio in and sends it to out queue
//  virtual bool start_listening();
  void listen();
//  virtual void stop_listening();
  virtual void pause_listening(bool pause) = 0;
//  virtual inline bool is_paused() const { return this->m_paused; }
  
  unsigned long get_sample_rate() const;
  unsigned int get_bytes_per_sample() const;
  /** Locks (recursively) m_audio_data for writing or writing-prevention. */
  // These lock functions are for no use anymore if listening thread is not recreated.
//  inline bool lock_audio_writing() { return pthread_mutex_lock(&this->m_lock) == 0; }
  /** No need for locking. You should stop listening before reseting! */
  virtual void reset();
  /** m_audio_data can only be modified inside the class. */
  inline const AUDIO_FORMAT* get_audio_data() const { return (AUDIO_FORMAT*)this->m_audio_data.data(); }
  inline unsigned long get_audio_data_size() const { return this->m_audio_data.size() / sizeof(AUDIO_FORMAT); }
//  inline const std::string* get_audio_data() const { return &this->m_audio_data; }
  /** Tells the size of data sent to the recognizer. */
  inline unsigned long get_read_cursor() const { return this->m_recognizer_cursor; }
//  inline void unlock_audio_writing() { pthread_mutex_unlock(&this->m_lock); }
  
protected:

//  void do_listening();
  virtual unsigned long read_input() = 0;
  
  std::string m_audio_data;
  OutQueueController *m_out_queue;
//  bool m_stop;
  
private:

//  static void* activate_thread(void *data);

//  pthread_t m_thread;
//  pthread_mutex_t m_lock;
//  pthread_mutexattr_t m_lock_attr;
//  bool m_thread_created;

  /** read cursor in bytes! */
  unsigned long m_recognizer_cursor;
//  bool m_paused;
};

#endif /*AUDIOINPUTCONTROLLER_HH_*/
