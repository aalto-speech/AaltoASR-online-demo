
#ifndef AUDIOINPUTCONTROLLER_HH_
#define AUDIOINPUTCONTROLLER_HH_

#include <iostream>
#include <string.h>
#include <queue>
#include "msg.hh"
#include "AudioInputStream.hh"
//#include "FileOutput.hh"

class AudioInputController
{
  
public:
  
  AudioInputController(msg::OutQueue *out_queue);
  virtual ~AudioInputController();
  
  virtual bool initialize();
  virtual void terminate();

  // Starts a new thread which listens audio in and sends it to out queue
  virtual bool start_listening();
  virtual void stop_listening();
  virtual void pause_listening(bool pause);
  
  unsigned long get_sample_rate() const;
  unsigned int get_bytes_per_sample() const;
  inline const std::string* get_audio_data() const { return &this->m_audio_data; };
  
protected:

  void do_listening();
  virtual unsigned long read_input() = 0;
  
  std::string m_audio_data;
  unsigned long m_read_cursor;
  msg::OutQueue *m_out_queue;
  bool m_stop;
  
private:

  static void* activate_thread(void *data);

//  AudioInputStream m_audio_input;
  pthread_t m_thread;
//  pthread_mutex_t *m_lock;
  bool m_paused;
};

#endif /*AUDIOINPUTCONTROLLER_HH_*/
