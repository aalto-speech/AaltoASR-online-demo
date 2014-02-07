
#ifndef WINDOWMESSAGEBOX_HH_
#define WINDOWMESSAGEBOX_HH_

#include "WindowChild.hh"

/** A general message box pop up window. */
class WindowMessageBox  :  public WindowChild
{
public:

  WindowMessageBox(const PG_Widget *parent, const std::string &title, const std::string &message, const std::string &button);
  WindowMessageBox(const PG_Widget *parent, const std::string &title, const std::string &message, const std::string &button1, const std::string &button2);
  virtual ~WindowMessageBox() { }

  virtual void initialize();

private:

  std::string m_message;

};

#endif /*WINDOWMESSAGEBOX_HH_*/
