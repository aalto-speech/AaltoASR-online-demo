
#ifndef WINDOWSTARTPROCESS_HH_
#define WINDOWSTARTPROCESS_HH_

#include "WindowWaitRecognizer.hh"
//#include "Process.hh"
//#include "msg.hh"
#include "RecognizerProcess.hh"

class WindowStartProcess  :  public WindowWaitRecognizer
{

public:

  WindowStartProcess(const PG_Widget *parent, RecognizerProcess *m_recognizer);
  virtual ~WindowStartProcess() { }

protected:

  virtual void do_opening();
  
  virtual void handle_broken_pipe();
  
  RecognizerProcess *m_recognizer;
//  Process *m_process;
//  msg::OutQueue *m_out_queue;
  
private:

//  void finish_process_and_queues();
//  void start_process();
//  void start_queues();

};

#endif /*WINDOWSTARTPROCESS_HH_*/
