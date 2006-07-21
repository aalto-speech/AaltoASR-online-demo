
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
  
//  virtual void handle_close_child_window(Window *child_window, int ret_val);
  bool handle_save_button(PG_Button *button);
  
  virtual void reset(bool reset_audio);
  
//  void save_audio_file();
  
private:

  AudioLineInputController *m_audio_input;
  
//  Window *m_savefile_window;
  
  PG_Button *m_save_button;
//  bool m_save_pressed;
};


#endif /*WINDOWMICROPHONERECOGNIZER_HH_*/
