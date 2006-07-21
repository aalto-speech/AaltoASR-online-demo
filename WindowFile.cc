
#include <assert.h>
#include "WindowFile.hh"

// This is to fasten debugging..
std::string WindowFile::last_file = "chunk.wav";

WindowFile::WindowFile(const PG_Widget *parent, const std::string &title)
  : WindowChild(parent, title, 400, 200, true, "OK", "Cancel")
{
  this->m_filename_textbox = NULL;
  this->m_text_label = NULL;
}

WindowFile::~WindowFile()
{
  delete this->m_filename_textbox;
  delete this->m_text_label;
}

void
WindowFile::initialize()
{
  WindowChild::initialize();
  
  this->m_text_label = new PG_Label(this->m_window,
                                    PG_Rect(10, 40, 150, 30),
                                    "File path:");
  this->m_filename_textbox = new PG_LineEdit(this->m_window,
                                             PG_Rect(10,
                                                     80,
                                                     this->m_window->my_width - 20,
                                                     25));

  this->m_window->AddChild(this->m_text_label);
  this->m_window->AddChild(this->m_filename_textbox);
  
  this->m_filename_textbox->SetText(WindowFile::last_file.data());
}

void
WindowFile::do_closing(int return_value)
{
  if (return_value == 1) {
    WindowFile::last_file = this->get_filename();
  }
}
