
#ifndef WINDOWMAIN_HH_
#define WINDOWMAIN_HH_

#include "undefine.hh"
#include <pgbutton.h>
#include "Window.hh"

class WindowMain  :  public Window
{

public:

  WindowMain() { }
  virtual ~WindowMain() { }

  virtual void initialize();
  
protected:

  bool handle_file_button();
  bool handle_microphone_button();
  bool handle_exit_button();
  
};

#endif /*WINDOWMAIN_HH_*/
