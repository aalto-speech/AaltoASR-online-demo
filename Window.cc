
#include "Window.hh"
#include "WindowMessageBox.hh"

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

  this->open();

  // run while in modal mode
  while(!this->m_end_run) {
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        this->end_running(0);
      PG_Application::ClearOldMousePosition();
      this->m_window->ProcessEvent(&event, true);
      PG_Application::DrawCursor();
    }
    else {
      this->do_running();
    }
  }
  
  this->close();
  
  return this->m_return_value;
}

void
Window::open()
{
  this->m_end_run = false;
  this->m_return_value = 1;
  this->do_opening();
  if (!this->m_end_run) {
    this->m_window->SetInputFocus();
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
  
int
Window::run_child_window(Window *child_window)
{
  int ret_val;

  this->m_window->EnableReceiver(false, true);
  ret_val = child_window->run_modal();
  this->m_window->EnableReceiver(true, true);

  if (ret_val == 0)
    this->end_running(0);
    
  return ret_val;
}

void
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
}

