
#ifndef WINDOWMICROPHONERECOGNIZER_HH_
#define WINDOWMICROPHONERECOGNIZER_HH_

#include "AudioLineInputController.hh"
#include "WindowRecognizer.hh"
#include <pgbutton.h>

class WindowMicrophoneRecognizer  :  public WindowRecognizer
{

public:
  
  WindowMicrophoneRecognizer(msg::InQueue *in_queue, OutQueueController *out_queue);
  virtual ~WindowMicrophoneRecognizer();
  
  virtual void initialize();

protected:

  virtual void do_opening();
  virtual void do_running();
  virtual void do_closing(int return_value);
  
  virtual AudioInputController* get_audio_input() { return this->m_audio_input; }
  
  bool handle_button(PG_Button *button);
  
  virtual void reset(bool reset_audio);
  
  void save_audio_file();
  
//  void do_reseting();

//  bool button_pressed(PG_Button *button);
  
private:

  AudioLineInputController *m_audio_input;
  
  PG_Button *m_save_button;
  bool m_save_pressed;
};


#endif /*WINDOWMICROPHONERECOGNIZER_HH_*/
