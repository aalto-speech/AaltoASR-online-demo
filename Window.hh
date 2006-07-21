
#ifndef WINDOW_HH_
#define WINDOW_HH_

#include <pgapplication.h>
#include <pgwindow.h>
//#include "WindowMessageBox.hh"


class Window  :  public SigC::Object
{

public:

  Window();
  virtual ~Window();

  int run_modal();
  
  virtual void initialize();
  
protected:

  enum ErrorType { ERROR_FATAL, ERROR_CLOSE, ERROR_NORMAL };
  void error(const std::string &message, ErrorType type);

  virtual void do_opening() { };
  virtual void do_running() { };
  virtual void do_closing(int return_value) { };
  
  void end_running(int return_value);
  
  virtual PG_Widget* create_window() { return new PG_Widget(NULL, PG_Rect(0, 0, PG_Application::GetScreenWidth(), PG_Application::GetScreenHeight()), false); }
  int run_child_window(Window *child_window);
  
  PG_Widget *m_window;
  
private:

  void open();
  void close();

  // This flag indicates that function do_running should end ASAP.
  bool m_end_run;
  int m_return_value;
  
};

#endif /*WINDOW_HH_*/
