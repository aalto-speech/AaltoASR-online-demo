
#include "WindowSettings.hh"
#include "Settings.hh"
#include "str.hh"

WindowSettings::WindowSettings(const PG_Widget *parent, msg::OutQueue *out_queue)
  : WindowChild(parent, "Settings", 350, 300, true, "OK", "Cancel")
{
  this->m_beam_edit = NULL;
  this->m_lmscale_edit = NULL;
  this->m_out_queue = out_queue;
}

void
WindowSettings::initialize()
{
  WindowChild::initialize();
  
  // Text labels.
  new PG_Label(this->m_window, PG_Rect(10, 50, 200, 20), "Enter parameters.");
  new PG_Label(this->m_window, PG_Rect(10, 100, 150, 20), "Beam (1-200):");
  new PG_Label(this->m_window, PG_Rect(10, 150, 150, 20), "LM-scale (1-100):");
  
  // Text edit boxes.
  this->m_beam_edit = new PG_LineEdit(this->m_window,
                                      PG_Rect(170, 100, 50, 20),
                                      "LineEdit",
                                      3);
  this->m_lmscale_edit = new PG_LineEdit(this->m_window,
                                         PG_Rect(170, 150, 50, 20),
                                         "LineEdit",
                                         3);
  
  // Write current settings into line edits.  
  char buffer[100];
  sprintf(buffer, "%d", Settings::beam);
  this->m_beam_edit->SetText(buffer);
  sprintf(buffer, "%d", Settings::lmscale);
  this->m_lmscale_edit->SetText(buffer);
}

bool
WindowSettings::do_ok()
{
  bool ok = true;
  int beam;
  int lmscale;
  
  beam = this->read_value(this->m_beam_edit, 1, 200, &ok);
  if (!ok) {
    this->error("Beam must be an integer between 1-200.", ERROR_NORMAL);
    return false;
  }

  lmscale = this->read_value(this->m_lmscale_edit, 1, 100, &ok);
  if (!ok) {
    this->error("LM-scale must be an integer between 1-100.", ERROR_NORMAL);
    return false;
  }

  // Send parameter change messages to recognizer.
  Settings::beam = beam;
  Settings::lmscale = lmscale;
  try {
    Settings::send_settings(this->m_out_queue);
  }
  catch(msg::ExceptionBrokenPipe) {
    this->end_running(-1);
    return false;
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
