
#ifndef WINDOWWAITRECOGNIZER_HH_
#define WINDOWWAITRECOGNIZER_HH_

#include "WindowChild.hh"
#include "msg.hh"

class WindowWaitRecognizer  :  public WindowChild
{
  
public:

  WindowWaitRecognizer(const PG_Widget *parent,
                       const std::string &title,
                       unsigned int width,
                       unsigned int height,
                       msg::InQueue *in_queue);

  virtual ~WindowWaitRecognizer() { }

protected:

  virtual void do_opening();
  // May throw msg::ExceptionBrokenPipe
  virtual void do_running();// throw(msg::ExceptionBrokenPipe);

  msg::InQueue *m_in_queue;
  
};

#endif /*WINDOWWAITRECOGNIZER_HH_*/
