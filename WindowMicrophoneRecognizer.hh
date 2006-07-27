
#ifndef WINDOWMICROPHONERECOGNIZER_HH_
#define WINDOWMICROPHONERECOGNIZER_HH_

#include "AudioLineInputController.hh"
#include "WindowRecognizer.hh"
#include <pgbutton.h>

class WindowMicrophoneRecognizer  :  public WindowRecognizer
{

public:
  
  WindowMicrophoneRecognizer(Process *process, msg::InQueue *in_queue, msg::OutQueue *out_queue);
  virtual ~WindowMicrophoneRecognizer();
  
  virtual void initialize();

protected:

  virtual void do_opening();
  virtual void do_closing(int return_value);
  
  virtual AudioInputController* get_audio_input() { return this->m_audio_input; }
  
  bool handle_save_button();
  
  virtual void reset(bool reset_audio);
  
private:

  AudioLineInputController *m_audio_input;
  
};


#endif /*WINDOWMICROPHONERECOGNIZER_HH_*/
