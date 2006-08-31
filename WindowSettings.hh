
#ifndef WINDOWSETTINGS_HH_
#define WINDOWSETTINGS_HH_

#include <pglineedit.h>
#include "WindowChild.hh"
#include "RecognizerProcess.hh"
#include "WidgetSpectrogram.hh"

/** Pop-up window to do some settings. */
class WindowSettings  :  public WindowChild
{
public:

  WindowSettings(const PG_Widget *parent,
                 RecognizerProcess *recognizer,
                 WidgetSpectrogram *spectrogram);
  virtual ~WindowSettings() { }

  virtual void initialize();
  
protected:

  // min and max not allowed
  float read_float_value(PG_LineEdit *line_edit, float min, float max, bool *ok);
  // min and max allowed
  long read_long_value(PG_LineEdit *line_edit, long min, long max, bool *ok);
  
  /** Tries to set the settings when ok is pressed. If flushing the out queue
   * fails, closes the window with signal -1. */
  virtual bool do_ok();

private:

  RecognizerProcess *m_recognizer;
  WidgetSpectrogram *m_spectrogram;

  // Some ParaGUI fields for text input.
  PG_LineEdit *m_beam_edit;
  PG_LineEdit *m_lmscale_edit;
  PG_LineEdit *m_exponent_edit;
  PG_LineEdit *m_suppressor_edit;
};

#endif /*WINDOWSETTINGS_HH_*/
