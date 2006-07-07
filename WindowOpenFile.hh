
#ifndef WINDOWOPENFILE_HH_
#define WINDOWOPENFILE_HH_

#include <pgbutton.h>
#include <pglineedit.h>
#include "Window.hh"
#include "AudioFileInputController.hh"

class WindowOpenFile  :  public Window
{
  
public:

  WindowOpenFile(int x, int y, AudioFileInputController *audio_input);
  virtual ~WindowOpenFile();

  virtual void initialize();
  virtual void do_opening();
  virtual void do_running();
  virtual void do_closing(int return_value);
  
  bool handle_button(PG_Button *button);
  
  virtual PG_Widget* create_window()
  {
    return new PG_Window(NULL, PG_Rect(this->m_x,this->m_y,300,200), "Open Audio File", PG_Window::MODAL);
  }
  
private:

  int m_x, m_y;
  AudioFileInputController *m_audio_input;
  PG_LineEdit *m_filename_textbox;
  PG_Button *m_ok_button;
  PG_Button *m_cancel_button;
  bool m_load;
  
};

#endif /*WINDOWOPENFILE_HH_*/
