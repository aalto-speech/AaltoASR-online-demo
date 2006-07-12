
#ifndef WINDOWRESET_HH_
#define WINDOWRESET_HH_

#include <pgwindow.h>
#include <pgbutton.h>
#include "WindowChild.hh"
#include "AudioInputController.hh"
#include "msg.hh"
#include "OutQueueController.hh"

/** Opens a window and sends resetting message to recognizer.
 * Window closes when it receives ready message from recognizer.
 * This way the end-user can really see the reseting happening instead
 * of a "dead" program. */
class WindowReset  :  public WindowChild
{

public:

  WindowReset(const PG_Widget *parent, msg::InQueue *in_queue, OutQueueController *out_queue);

protected:

  virtual void do_opening();
  virtual void do_running();

//  virtual PG_Widget* create_window();

private:

//  const PG_Widget *m_parent;
  msg::InQueue *m_in_queue;
  OutQueueController *m_out_queue;
  
};

#endif /*WINDOWRESET_HH_*/
