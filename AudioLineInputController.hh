
#ifndef AUDIOLINEINPUTCONTROLLER_HH_
#define AUDIOLINEINPUTCONTROLLER_HH_


#include <iostream>
#include <string.h>
#include <queue>
#include "msg.hh"
#include "AudioInputController.hh"

class AudioLineInputController  :  public AudioInputController
{
  
public:
  
  AudioLineInputController(msg::OutQueue *out_queue);
  virtual ~AudioLineInputController();
  
  // Starts a new thread which listens audio in and sends it to out queue
  virtual void pause_listening(bool pause);

  virtual unsigned long get_audio_cursor() const { return this->get_audio_data_size(); }
  
  virtual void reset_cursors();

protected:

  virtual bool open_stream(bool input_stream) { return AudioInputController::open_stream(true); }

  virtual void read_input();

private:

  AudioBuffer m_input_buffer;
  bool m_paused;
};


#endif /*AUDIOLINEINPUTCONTROLLER_HH_*/
