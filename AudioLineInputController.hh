
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
  
  AudioLineInputController(OutQueueController *out_queue);
  virtual ~AudioLineInputController();
  
  virtual bool initialize();
  virtual void terminate();

  // Starts a new thread which listens audio in and sends it to out queue
//  virtual bool start_listening();
//  virtual void stop_listening();
  virtual void pause_listening(bool pause);
  
  virtual void reset_cursors();

protected:

  virtual unsigned long read_input();

private:

  AudioInputStream m_audio_input;
  bool m_paused;
};


#endif /*AUDIOLINEINPUTCONTROLLER_HH_*/
