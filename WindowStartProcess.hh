
#ifndef WINDOWSTARTPROCESS_HH_
#define WINDOWSTARTPROCESS_HH_

#include "WindowWaitRecognizer.hh"
//#include "Settings.hh"
#include "Process.hh"
#include "msg.hh"

class WindowStartProcess  :  public WindowWaitRecognizer
{

public:

  WindowStartProcess(const PG_Widget *parent, Process *process,
                     msg::InQueue *in_queue, msg::OutQueue *out_queue);

protected:

  virtual void do_opening();
  virtual void do_running();
  
  Process *m_process;
  msg::OutQueue *m_out_queue;
  
private:

  void finish_process_and_queues();
  void start_process();
  void start_queues();

};

#endif /*WINDOWSTARTPROCESS_HH_*/
