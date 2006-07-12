
#ifndef WINDOWSETTINGS_HH_
#define WINDOWSETTINGS_HH_

#include <pglabel.h>
#include <pglineedit.h>
#include "WindowChild.hh"
#include "OutQueueController.hh"

class WindowSettings  :  public WindowChild
{
public:

  WindowSettings(const PG_Widget *parent, OutQueueController *out_queue);
  virtual ~WindowSettings();

  virtual void initialize();
  
protected:

  long read_value(PG_LineEdit *line_edit, long min, long max, bool *ok);

  virtual bool do_ok();

private:

  OutQueueController *m_out_queue;

  PG_Label *m_text_label;
  PG_Label *m_beam_label;
  PG_Label *m_lmscale_label;
  PG_LineEdit *m_beam_edit;
  PG_LineEdit *m_lmscale_edit;
};

#endif /*WINDOWSETTINGS_HH_*/
