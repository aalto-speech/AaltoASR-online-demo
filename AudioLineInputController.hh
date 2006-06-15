
#ifndef AUDIOLINEINPUTCONTROLLER_HH_
#define AUDIOLINEINPUTCONTROLLER_HH_


#include <iostream>
#include <string.h>
#include <queue>
#include "msg.hh"
#include "AudioInputStream.hh"
#include "AudioInputController.hh"

class AudioLineInputController  :  public AudioInputController
{
  
public:
  
  AudioLineInputController(msg::OutQueue *out_queue);
  virtual ~AudioLineInputController();
  
  virtual bool initialize();
  virtual void terminate();

  // Starts a new thread which listens audio in and sends it to out queue
  virtual bool start_listening();
  virtual void stop_listening();
  virtual void pause_listening(bool pause);
  
//  unsigned long get_sample_rate() const;
//  unsigned int get_bytes_per_sample() const;
//  inline const std::string* get_audio_data() const { return &this->m_audio_data; };
  
protected:

//  virtual void do_listening();
  virtual unsigned long read_input();

private:

  AudioInputStream m_audio_input;
//  unsigned long m_read_cursor;
  bool m_paused;
};


#endif /*AUDIOLINEINPUTCONTROLLER_HH_*/
