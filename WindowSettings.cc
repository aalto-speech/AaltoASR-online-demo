
#include "WindowSettings.hh"
#include "str.hh"

WindowSettings::WindowSettings(const PG_Widget *parent, RecognizerProcess *recognizer)
  : WindowChild(parent, "Settings", 350, 300, true, "OK", "Cancel")
{
  this->m_beam_edit = NULL;
  this->m_lmscale_edit = NULL;
//  this->m_out_queue = out_queue;
  this->m_recognizer = recognizer;
}

void
WindowSettings::initialize()
{
  std::string text;
  WindowChild::initialize();
  
  // Text labels.
  new PG_Label(this->m_window, PG_Rect(10, 50, 200, 20), "Enter parameters.");
  text = str::fmt(40, "Beam (%u-%u):", RecognizerProcess::MIN_BEAM,
                                       RecognizerProcess::MAX_BEAM);
  new PG_Label(this->m_window, PG_Rect(10, 100, 150, 20), text.data());
  text = str::fmt(40, "LM-scale (%u-%u):", RecognizerProcess::MIN_LMSCALE,
                                           RecognizerProcess::MAX_LMSCALE);
  new PG_Label(this->m_window, PG_Rect(10, 150, 150, 20), text.data());
  
  // Text edit boxes.
  this->m_beam_edit = new PG_LineEdit(this->m_window,
                                      PG_Rect(170, 100, 50, 20),
                                      "LineEdit",
                                      3);
  this->m_lmscale_edit = new PG_LineEdit(this->m_window,
                                         PG_Rect(170, 150, 50, 20),
                                         "LineEdit",
                                         3);
  
  if (this->m_recognizer) {
    // Write current settings into line edits.  
    text = str::fmt(10, "%d", this->m_recognizer->get_beam());
    this->m_beam_edit->SetText(text.data());
    text = str::fmt(10, "%d", this->m_recognizer->get_lmscale());
    this->m_lmscale_edit->SetText(text.data());
  }
}

bool
WindowSettings::do_ok()
{
  bool ok = true;
  int beam;
  int lmscale;
  
  beam = this->read_value(this->m_beam_edit, 1, 200, &ok);
  if (!ok) {
    this->error(str::fmt(50,
                         "Beam must be an integer between %u-%u.",
                         RecognizerProcess::MIN_BEAM,
                         RecognizerProcess::MAX_BEAM),
                ERROR_NORMAL);
    return false;
  }

  lmscale = this->read_value(this->m_lmscale_edit, 1, 100, &ok);
  if (!ok) {
    this->error(str::fmt(50,
                         "LM-scale must be an integer between %u-%u.",
                         RecognizerProcess::MIN_LMSCALE,
                         RecognizerProcess::MAX_LMSCALE),
                ERROR_NORMAL);
    return false;
  }

  // Send parameter change messages to recognizer.
  if (this->m_recognizer) {
    this->m_recognizer->set_beam(beam);
    this->m_recognizer->set_lmscale(lmscale);
    try {
      this->m_recognizer->get_out_queue()->flush();
//      this->m_recognizer->send_settings();
    }
    catch(msg::ExceptionBrokenPipe) {
      this->end_running(-1);
      return false;
    }
  }

  return true;
}

long
WindowSettings::read_value(PG_LineEdit *line_edit, long min, long max, bool *ok)
{
  std::string text = line_edit->GetText();
  long value = str::str2long(&text, ok);

  if (!ok)
    return 0;
    
  if (value < min || value > max) {
    *ok = false;
    return 0;
  }
  
  return value;
}
