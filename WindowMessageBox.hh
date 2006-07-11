
#ifndef WINDOWMESSAGEBOX_HH_
#define WINDOWMESSAGEBOX_HH_

//#include <pglabel.h>
#include <pgrichedit.h>
#include <pgbutton.h>
#include "Window.hh"

class WindowMessageBox  :  public Window
{
public:

  WindowMessageBox(const PG_Widget *parent, const std::string &title, const std::string &message, const std::string &button);
  WindowMessageBox(const PG_Widget *parent, const std::string &title, const std::string &message, const std::string &button1, const std::string &button2);
  virtual ~WindowMessageBox();

  virtual void initialize();
//  inline bool is_closed() const { return this->m_closed; }

protected:

  virtual void do_running() { }
  virtual PG_Widget* create_window();// { return new PG_Widget(NULL, PG_Rect(0, 0, PG_Application::GetScreenWidth(), PG_Application::GetScreenHeight()), false); }

  virtual bool handle_button(PG_Button *button);
  
private:

  const PG_Widget *m_parent;
  std::string m_message;
  std::string m_title;
  std::string m_button1_text;
  std::string m_button2_text;
  bool m_two_buttons;
  
  PG_Button *m_button1;
  PG_Button *m_button2;
  PG_RichEdit *m_text_label;
//  bool m_closed;

};

#endif /*WINDOWMESSAGEBOX_HH_*/
