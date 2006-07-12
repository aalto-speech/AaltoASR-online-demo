
#include "WindowSettings.hh"
#include "Settings.hh"
#include "str.hh"

WindowSettings::WindowSettings(const PG_Widget *parent, OutQueueController *out_queue)
  : WindowChild(parent, "Settings", 350, 300, true, "OK", "Cancel")
{
  this->m_text_label = NULL;
  this->m_beam_label = NULL;
  this->m_lmscale_label = NULL;
  this->m_beam_edit = NULL;
  this->m_lmscale_edit = NULL;
}

WindowSettings::~WindowSettings()
{
  delete this->m_text_label;
  delete this->m_beam_label;
  delete this->m_lmscale_label;
  delete this->m_beam_edit;
  delete this->m_lmscale_edit;
}

void
WindowSettings::initialize()
{
  WindowChild::initialize();
  
  char buffer[100];
  
  this->m_text_label = new PG_Label(this->m_window,
                                    PG_Rect(10, 50, 200, 20),
                                    "Enter parameters.");
  this->m_beam_label = new PG_Label(this->m_window,
                                    PG_Rect(10, 100, 150, 20),
                                    "Beam (1-200):");
  this->m_lmscale_label = new PG_Label(this->m_window,
                                       PG_Rect(10, 150, 150, 20),
                                       "LM-scale (1-100):");
  this->m_beam_edit = new PG_LineEdit(this->m_window,
                                      PG_Rect(170, 100, 50, 20),
                                      "LineEdit",
                                      3);
  this->m_lmscale_edit = new PG_LineEdit(this->m_window,
                                         PG_Rect(170, 150, 50, 20),
                                         "LineEdit",
                                         3);
  
  this->m_window->AddChild(this->m_text_label);
  this->m_window->AddChild(this->m_beam_label);
  this->m_window->AddChild(this->m_lmscale_label);
  this->m_window->AddChild(this->m_beam_edit);
  this->m_window->AddChild(this->m_lmscale_edit);
  
  sprintf(buffer, "%d", Settings::get_beam());
  this->m_beam_edit->SetText(buffer);
  sprintf(buffer, "%d", Settings::get_lmscale());
  this->m_lmscale_edit->SetText(buffer);
}

bool
WindowSettings::do_ok()
{
  bool ok = true;
  unsigned int beam;
  unsigned int lmscale;
  
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

  Settings::set_beam(beam);
  Settings::set_lmscale(lmscale);
  
  if (this->m_out_queue) {
    // Send parameter change messages to recognizer.
    // ...
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
