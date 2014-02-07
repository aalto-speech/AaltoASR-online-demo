
#include <assert.h>
#include "WindowFile.hh"

WindowFile::WindowFile(const PG_Widget *parent, const std::string &title)
  : WindowChild(parent, title, 400, 200, true, true, "OK", "Cancel")
{
  this->m_filename_textbox = NULL;
}

void
WindowFile::initialize()
{
  WindowChild::initialize();
  
  PG_Label *text_label = new PG_Label(this->m_window,
                                    PG_Rect(10, 40, 150, 30),
                                    "File path:");
  this->m_filename_textbox = new PG_LineEdit(this->m_window,
                                             PG_Rect(10,
                                                     80,
                                                     this->m_window->my_width - 20,
                                                     25));

  this->m_window->AddChild(text_label);
  this->m_window->AddChild(this->m_filename_textbox);
  
}

void
WindowFile::focus_textbox()
{
  this->m_filename_textbox->EditBegin();
  // Have to set the cursor position, because ParaGUI "forgets" the position
  // when input focus is lost.
  this->m_filename_textbox->SetCursorPos(this->get_filename().size());
}

void
WindowFile::do_opening()
{
  this->focus_textbox();
}

bool
WindowFile::file_exists()
{
  FILE *file = fopen(this->get_filename().data(), "r");
  if (file != NULL) {
    fclose(file);
    return true;
  }
  return false;
}
