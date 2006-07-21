
#ifndef WINDOWMAIN_HH_
#define WINDOWMAIN_HH_

#include <pgbutton.h>
#include "Window.hh"

class WindowMain  :  public Window
{

public:

  WindowMain();
  virtual ~WindowMain();

  virtual void initialize();
  
protected:

//  virtual void do_running();

  bool handle_file_button(PG_Button *button);
  bool handle_microphone_button(PG_Button *button);
  bool handle_exit_button(PG_Button *button);
  
private:

  PG_Button *m_file_button;
  PG_Button *m_microphone_button;
  PG_Button *m_exit_button;

};

#endif /*WINDOWMAIN_HH_*/
