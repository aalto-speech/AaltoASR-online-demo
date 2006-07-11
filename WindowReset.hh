
#ifndef WINDOWRESET_HH_
#define WINDOWRESET_HH_

#include <pgwindow.h>
#include <pgbutton.h>
#include "Window.hh"
#include "AudioInputController.hh"
#include "msg.hh"
#include "OutQueueController.hh"

/** Opens a window and sends resetting message to recognizer.
 * Window closes when it receives ready message from recognizer.
 * This way the end-user can really see the reseting happening instead
 * of a "dead" program. */
class WindowReset  :  public Window
{

public:

  WindowReset(const PG_Widget *parent, msg::InQueue *in_queue, OutQueueController *out_queue);
  virtual ~WindowReset();

//  virtual void initialize();

protected:

  virtual void do_opening();
  virtual void do_running();
//  virtual void do_closing();

  virtual PG_Widget* create_window();
 
//  bool button_pressed(PG_Button *button);
     
private:

//  PG_Button *m_ok_button;
  
//  AudioInputController *m_audio_input;
  const PG_Widget *m_parent;
  msg::InQueue *m_in_queue;
  OutQueueController *m_out_queue;
  
};

#endif /*WINDOWRESET_HH_*/
