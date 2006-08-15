
#include "Window.hh"
#include "WindowMessageBox.hh"
#include <pgapplication.h>

Window::Window()
{
  this->m_window = NULL;
}

Window::~Window()
{
  this->do_closing(0);
  delete this->m_window;
}

void
Window::initialize()
{
  // This will do memory allocation.
  this->m_window = this->create_window();
}

int
Window::run_modal()
{
  SDL_Event event;

  // Opening procedures.
  this->open();

  // Run until requested to end.
  while(!this->m_end_run) {
    if (SDL_PollEvent(&event)) {
      PG_Application::ClearOldMousePosition();
      if (event.type == SDL_QUIT)
        this->end_running(0);
      else
        this->m_window->ProcessEvent(&event, true);
      PG_Application::DrawCursor();
    }
    else {
      // When idle, allow window to perform its own operations.
      this->do_running();
    }
  }
  
  // Closing procedures.
  this->close();
  
  return this->m_return_value;
}

void
Window::open()
{
  this->m_end_run = false;
  this->m_return_value = 1;
  this->m_window->SetInputFocus();
  this->do_opening();
  if (!this->m_end_run) {
    this->m_window->Show(false);
  }
}

void
Window::close()
{
  this->m_window->Hide(false);
  this->do_closing(this->m_return_value);
}

void
Window::end_running(int return_value)
{
  if (this->m_return_value != 0)
    this->m_return_value = return_value;
    
  this->m_end_run = true;
}

void
Window::pause_window_functionality(bool pause)
{
  this->m_window->EnableReceiver(!pause, true);
}
  
int
Window::run_child_window(Window *child_window)
{
  int ret_val;

  this->pause_window_functionality(true);
  ret_val = child_window->run_modal();

  if (ret_val == 0)
    this->end_running(0);

  if (!this->m_end_run)    
    this->pause_window_functionality(false);
    
  return ret_val;
}

PG_Widget*
Window::create_window()
{
  // Creates a full screen window. (Full screen in application window.)
  return new PG_Widget(NULL,
                       PG_Rect(0,
                               0,
                               PG_Application::GetScreenWidth(),
                               PG_Application::GetScreenHeight()),
                       false);
}

bool
Window::error(const std::string &message, ErrorType type)
{
  fprintf(stderr, "%s\n", message.data());

  WindowMessageBox window(this->m_window, "Error", message, "OK");
  window.initialize();
  this->run_child_window(&window);

  // Error type handling.
  if (type == ERROR_CLOSE) {
    this->end_running(1);
  }
  else if (type == ERROR_FATAL) {
    this->end_running(0);
  }
  return !this->m_end_run;
}

