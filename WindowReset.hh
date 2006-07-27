
#ifndef WINDOWRESET_HH_
#define WINDOWRESET_HH_

#include "WindowWaitRecognizer.hh"

class WindowReset  :  public WindowWaitRecognizer
{

public:

  WindowReset(const PG_Widget *parent,
              msg::InQueue *in_queue,
              msg::OutQueue *out_queue);
  virtual ~WindowReset() { }

protected:

  virtual void do_opening() throw(msg::ExceptionBrokenPipe);
  
  msg::OutQueue *m_out_queue;

};

#endif /*WINDOWRESET_HH_*/
