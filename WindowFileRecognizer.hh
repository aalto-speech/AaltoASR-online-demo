
#ifndef WINDOWFILERECOGNIZER_HH_
#define WINDOWFILERECOGNIZER_HH_

#include <pgbutton.h>
#include "AudioFileInputController.hh"
#include "WindowRecognizer.hh"

class WindowFileRecognizer  :  public WindowRecognizer
{

public:
  
  WindowFileRecognizer(Process *process, msg::InQueue *in_queue, msg::OutQueue *out_queue);
  virtual ~WindowFileRecognizer();
  
  virtual void initialize();
  
protected:

  virtual void do_opening();
  virtual void do_running();
  virtual void do_closing(int return_value);
  
  virtual AudioInputController* get_audio_input() { return this->m_audio_input; }
  
  virtual void reset(bool reset_audio);
  
  void open_audio_file();
  
//  bool handle_button(PG_Button *button);
  bool handle_open_button(PG_Button *button);
//  virtual void handle_close_child_window(Window *child_window, int ret_val);
  
private:

//  Window *m_openfile_window;
  AudioFileInputController *m_audio_input;
  PG_Button *m_open_button;
//  bool m_open_file;
};

#endif /*WINDOWFILERECOGNIZER_HH_*/
