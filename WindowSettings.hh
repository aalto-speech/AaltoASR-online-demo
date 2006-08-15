
#ifndef WINDOWSETTINGS_HH_
#define WINDOWSETTINGS_HH_

#include <pglineedit.h>
#include "WindowChild.hh"
//#include "msg.hh"
#include "RecognizerProcess.hh"

class WindowSettings  :  public WindowChild
{
public:

  WindowSettings(const PG_Widget *parent, RecognizerProcess *recognizer);
  virtual ~WindowSettings() { }

  virtual void initialize();
  
protected:

  long read_value(PG_LineEdit *line_edit, long min, long max, bool *ok);
  
  virtual bool do_ok();

private:

//  msg::OutQueue *m_out_queue;
  RecognizerProcess *m_recognizer;

  PG_LineEdit *m_beam_edit;
  PG_LineEdit *m_lmscale_edit;
};

#endif /*WINDOWSETTINGS_HH_*/
