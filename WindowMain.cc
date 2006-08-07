
#include "WindowMain.hh"
#include <pgbutton.h>

void
WindowMain::initialize()
{
  Window::initialize();
  
  const unsigned int center_x =
    this->m_window->my_xpos + this->m_window->my_width / 2;
  // Use dividor 2.5 instead of 2.0 to move buttons a bit upper so it's
  // visually more comfortable.
  const unsigned int center_y =
    (unsigned int)((this->m_window->my_ypos + this->m_window->my_height / 2.5));
  const unsigned int width = 300;
  const unsigned int height = 80;
  const unsigned int space = 50;
  const unsigned int left = center_x - width / 2;
  const unsigned int top = center_y - (3 * height + 2 * space) / 2;
  
  PG_Button *file_button =
    new PG_Button(this->m_window,
                  PG_Rect(left, top + 0 * (height + space), width, height),
                  "Recognize audio file");
  PG_Button *microphone_button =
    new PG_Button(this->m_window,
                  PG_Rect(left, top + 1 * (height + space), width, height),
                  "Recognize microphone input");
  PG_Button *exit_button =
    new PG_Button(this->m_window,
                  PG_Rect(left, top + 2 * (height + space), width, height),
                  "Exit program");
  
  this->m_window->AddChild(file_button);
  this->m_window->AddChild(microphone_button);
  this->m_window->AddChild(exit_button);

  file_button->sigClick.connect(slot(*this, &WindowMain::handle_file_button));
  microphone_button->sigClick.connect(slot(*this, &WindowMain::handle_microphone_button));
  exit_button->sigClick.connect(slot(*this, &WindowMain::handle_exit_button));
}

bool
WindowMain::handle_file_button()
{
  this->end_running(1);
  return true;
}

bool
WindowMain::handle_microphone_button()
{
  this->end_running(2);
  return true;
}

bool
WindowMain::handle_exit_button()
{
  this->end_running(0);
  return true;
}
