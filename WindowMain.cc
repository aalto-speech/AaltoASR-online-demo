
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
  
  this->m_window->AddChild(this->m_exit_button);
  this->m_window->AddChild(this->m_microphone_button);
  this->m_window->AddChild(this->m_file_button);

  this->m_exit_button->sigClick.connect(slot(*this, &WindowMain::button_pressed));
  this->m_microphone_button->sigClick.connect(slot(*this, &WindowMain::button_pressed));
  this->m_file_button->sigClick.connect(slot(*this, &WindowMain::button_pressed));
}

bool
WindowMain::button_pressed(PG_Button *button)
{
  if (button == this->m_file_button) {
    this->close(1);
  }
  else if (button == this->m_microphone_button) {
    this->close(2);
  }
  else if (button == this->m_exit_button) {
    this->quit();
  }
  return true;
}

//*
void
WindowMain::do_running()
{
  pthread_yield();
}
/*
bool
WindowMain::eventMouseMotion(const SDL_MouseMotionEvent *motion)
{
//  fprintf(stderr, "Mouse event.\n");
  return true;  
}

bool
WindowMain::eventKeyDown(const SDL_KeyboardEvent *key)
{
  fprintf(stderr, "WindowMain::eventKeyDown\n");
  
  this->close();
  // Exit on Esc.
  if (key->keysym.sym == SDLK_ESCAPE)
    this->quit();

  return true;
}
//*/
