
#include "WindowMain.hh"

WindowMain::WindowMain()
{
  this->m_file_button = NULL;
  this->m_microphone_button = NULL;
  this->m_exit_button = NULL;
}

WindowMain::~WindowMain()
{
  delete this->m_exit_button;
  delete this->m_microphone_button;
  delete this->m_file_button;
}

void
WindowMain::initialize()
{
  Window::initialize();
  
  this->m_file_button = new PG_Button(this->m_window, PG_Rect(10,10,150,50), "File recognition");
  this->m_microphone_button = new PG_Button(this->m_window, PG_Rect(10,110,150,50), "Mic");
  this->m_exit_button = new PG_Button(this->m_window, PG_Rect(10,250,150,50), "Exit", PG_Button::CLOSE);
  
  this->m_window->AddChild(this->m_file_button);
  this->m_window->AddChild(this->m_microphone_button);
  this->m_window->AddChild(this->m_exit_button);

  this->m_file_button->sigClick.connect(slot(*this, &WindowMain::handle_file_button));
  this->m_microphone_button->sigClick.connect(slot(*this, &WindowMain::handle_microphone_button));
  this->m_exit_button->sigClick.connect(slot(*this, &WindowMain::handle_exit_button));
}

bool
WindowMain::handle_file_button(PG_Button *button)
{
  this->end_running(1);
  return true;
}

bool
WindowMain::handle_microphone_button(PG_Button *button)
{
  this->end_running(2);
  return true;
}

bool
WindowMain::handle_exit_button(PG_Button *button)
{
  this->end_running(0);
  return true;
}
