
//#include <pgapplication.h>
//#include <pgmessagebox.h>
#include "Window.hh"
#include "WindowMessageBox.hh"

Window::Window()
//  : PG_Widget(NULL, PG_Rect(0, 0, PG_Application::GetScreenWidth(), PG_Application::GetScreenHeight()), false)
//  : PG_Window(NULL, PG_Rect(0, 0, PG_Application::GetScreenWidth(), PG_Application::GetScreenHeight()), "", PG_Window::MODAL, "none", 0)
{
//  this->m_error_message.clear();
//  pthread_mutex_init(&this->m_error_lock, NULL);

  this->m_child_window = NULL;
  pthread_mutex_init(&this->m_childwindow_lock, NULL);

  this->m_window = NULL;
}

Window::~Window()
{
//  pthread_mutex_destroy(&this->m_error_lock);
  pthread_mutex_destroy(&this->m_childwindow_lock);
  delete this->m_window;
}

void
Window::initialize()
{
  // This will do memory allocation.
  this->m_window = this->create_window();
}
/*
PG_Widget*
Window::create_window()
{
  return new PG_Widget(NULL, PG_Rect(0, 0, PG_Application::GetScreenWidth(), PG_Application::GetScreenHeight()), false);
}
//*/
int
Window::run()
{
  this->do_opening();
/*
  if (this->m_error_message.length() > 0) {
    this->show_error_messagebox();
  }
//*/  
  if (!this->m_end_run) {
    this->m_window->SetInputFocus();
  
    this->m_window->Show(false);
    while (!this->m_end_run) {
      this->do_running();
      /*
      if (this->m_error_message.length() > 0) {
        this->show_error_messagebox();
      }
      //*/
    }
    this->m_window->Hide(false);
  }
  
  this->do_closing(this->m_return_value);

  return this->m_return_value;
}

void
Window::open()
{
  this->m_end_run = false;
  this->m_return_value = true;
}

void
Window::close(int return_value)
{
  this->m_return_value = return_value;
  this->m_end_run = true;
  // TODO: tarviiko tätä signaalia välittää lapselle...
  /*
  pthread_mutex_lock(&this->m_childwindow_lock);
  if (this->m_child_window) {
    this->m_child_window->close();
  }
  pthread_mutex_unlock(&this->m_childwindow_lock);
  //*/
}

void
Window::quit()
{
  this->m_return_value = 0;
  this->m_end_run = true;

  // Send quit signal to child window if one exists.
  pthread_mutex_lock(&this->m_childwindow_lock);
  if (this->m_child_window) {
    this->m_child_window->quit();
  }
  pthread_mutex_unlock(&this->m_childwindow_lock);
//  fprintf(stderr, "Window quit function\n");
//  this->m_application.Quit();
}

int
Window::run_child_window(Window *child_window)
{
  int ret_val;

  pthread_mutex_lock(&this->m_childwindow_lock);
  this->m_child_window = child_window;  
  child_window->open();
  pthread_mutex_unlock(&this->m_childwindow_lock);

  this->m_window->EnableReceiver(false, true);
  ret_val = child_window->run();
  this->m_window->EnableReceiver(true, true);

  pthread_mutex_lock(&this->m_childwindow_lock);
  this->m_child_window = NULL;
  pthread_mutex_unlock(&this->m_childwindow_lock);
  
  return ret_val;
}

void
Window::error(const std::string &message, ErrorType type)
{
  WindowMessageBox *window = new WindowMessageBox(this->m_window, "Error", message, "OK");
  fprintf(stderr, "%s\n", message.data());

  window->initialize();
  this->run_child_window(window);
/*  window->Show(true);
  while (!window->is_closed() && !this->m_end_run) {
    pthread_yield();
  }
  window->Hide(true);
  this->m_window->EnableReceiver(true, true);
//*/
  delete window;

  // Error type handling.
  if (type == ERROR_CLOSE) {
    this->close();
  }
  else if (type == ERROR_FATAL) {
    this->quit();
  }
/*
  // TODO: Tarvitseeko tätä virheiden käsittelyä tehdä näin "vaikeasti" säikeiden
  // takia? Eli kutsuuko tätä funktiota kuitenkin vain aina sama main thread,
  // jolloin voitaisiin suoraan kutsua show_error_messageboxia?
  if (pthread_mutex_lock(&this->m_error_lock) == 0) {
    this->m_error_message = message;
    this->m_error_type = type;
    pthread_mutex_unlock(&this->m_error_lock);
  }
  else {
    fprintf(stderr, "Window couldn't register error message: mutex lock failed.\n");
  }
  //*/
}
/*
void
Window::show_error_messagebox()
{
  if (pthread_mutex_lock(&this->m_error_lock) != 0) {
    fprintf(stderr, "Window couldn't show error messagebox: mutex lock failed.\n");
    return;
  }
  // Copy the error data for thread safety.
  int type = this->m_error_type;
  std::string message = this->m_error_message;
  MessageBox *window = new MessageBox(this->m_window, message);
  this->m_error_message.clear();
  pthread_mutex_unlock(&this->m_error_lock);

  this->m_window->EnableReceiver(false, true);
  window->Show(true);
  while (!window->is_closed() && !this->m_end_run) {
    pthread_yield();
  }
  window->Hide(true);
  delete window;
  this->m_window->EnableReceiver(true, true);

  // Error type handling.
  if (type == ERROR_CLOSE) {
    this->close();
  }
  else if (type == ERROR_FATAL) {
    this->quit();
  }
}
//*/


