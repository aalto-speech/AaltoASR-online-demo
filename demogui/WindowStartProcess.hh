
#ifndef WINDOWSTARTPROCESS_HH_
#define WINDOWSTARTPROCESS_HH_

#include "WindowChild.hh"
#include "RecognizerProcess.hh"

/** A window which finishes the recognizer process if running and restarts it.
 * Window closes only if the start was successful or exits the whole program
 * if user decides not to retry the start. */
class WindowStartProcess  :  public WindowChild
{

public:

  WindowStartProcess(const PG_Widget *parent, RecognizerProcess *m_recognizer);
  virtual ~WindowStartProcess() { }

protected:

  /** Finish and restart the process. */
  virtual void do_opening();
  /** Waits until the recognizer sends ready message. If process is disconnected
   * asks whether to retry or exit. */
  virtual void do_running();
  
  /** If process is disconnected, ask whether to retry. */
  virtual void handle_broken_pipe();
  
  RecognizerProcess *m_recognizer;
  
};

#endif /*WINDOWSTARTPROCESS_HH_*/
