
#include "WindowTextEdit.hh"

WindowTextEdit::WindowTextEdit(const PG_Widget *parent,
                               const std::string &title,
                               Uint16 width,
                               Uint16 height,
                               std::string *text)
  : WindowChild(parent, title, width, height, true, false, "OK", "Cancel")
{
  this->m_text = text;
  this->m_text_area = NULL;
}

void
WindowTextEdit::initialize()
{
  WindowChild::initialize();

  PG_Rect rect(5, 30, this->m_window->my_width - 10, this->m_window->my_height - 100);
  this->m_text_area = new WidgetMultiLineEdit(this->m_window, rect);
  if (this->m_text)
    this->m_text_area->SetText(this->m_text->data());
}

bool
WindowTextEdit::do_ok()
{
  if (this->m_text) {
    *this->m_text = this->m_text_area->GetText();
  }
  return true;
}
