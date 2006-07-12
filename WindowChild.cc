
#include "WindowChild.hh"

WindowChild::WindowChild(const PG_Widget *parent, const std::string &title, Uint16 width, Uint16 height, bool close)
  : m_parent(parent), m_width(width), m_height(height)
{
  this->m_title = title;
  this->m_ok_button = NULL;
  this->m_cancel_button = NULL;
  this->m_close = close;
  this->m_button_count = 0;
  this->m_ok_pressed = false;
}

WindowChild::WindowChild(const PG_Widget *parent, const std::string &title, Uint16 width, Uint16 height, bool close, const std::string &ok_text)
  : m_parent(parent), m_width(width), m_height(height)
{
  this->m_title = title;
  this->m_ok_button = NULL;
  this->m_cancel_button = NULL;
  this->m_close = close;
  this->m_ok_text = ok_text;
  this->m_button_count = 1;
  this->m_ok_pressed = false;
}

WindowChild::WindowChild(const PG_Widget *parent, const std::string &title, Uint16 width, Uint16 height, bool close, const std::string &ok_text, const std::string &cancel_text)
  : m_parent(parent), m_width(width), m_height(height)
{
  this->m_title = title;
  this->m_ok_button = NULL;
  this->m_cancel_button = NULL;
  this->m_close = close;
  this->m_ok_text = ok_text;
  this->m_cancel_text = cancel_text;
  this->m_button_count = 2;
  this->m_ok_pressed = false;
}

WindowChild::~WindowChild()
{
  delete this->m_ok_button;
  delete this->m_cancel_button;
}

void
WindowChild::initialize()
{
  Window::initialize();
  
  PG_Rect rect1, rect2;
  
  if (this->m_button_count == 1) {
    rect1.SetRect(this->m_window->my_width / 2 - 30,
                  this->m_window->my_height - 40,
                  60,
                  30);
  }
  else if (this->m_button_count == 2) {
    rect1.SetRect(this->m_window->my_width / 2 - 70,
                  this->m_window->my_height - 40,
                  60,
                  30);

    rect2.SetRect(this->m_window->my_width / 2 + 10,
                  this->m_window->my_height - 40,
                  60,
                  30);
  }
  
  if (this->m_button_count >= 1) {
    this->m_ok_button = new PG_Button(this->m_window, rect1, this->m_ok_text.data());
    this->m_ok_button->sigClick.connect(slot(*this, &WindowChild::handle_button));
    this->m_window->AddChild(this->m_ok_button);

    if (this->m_button_count >= 2) {
      this->m_cancel_button = new PG_Button(this->m_window, rect2, this->m_cancel_text.data());
      this->m_cancel_button->sigClick.connect(slot(*this, &WindowChild::handle_button));
      this->m_window->AddChild(this->m_cancel_button);
    }
  }

}

void
WindowChild::do_running()
{
  if (this->m_ok_pressed) {
    this->m_ok_pressed = false;
    if (this->do_ok())
      this->close(1);
  }
}

bool
WindowChild::handle_button(PG_Button *button)
{
  if (button == this->m_ok_button) {
    this->m_ok_pressed = true;
  }
  if (button == this->m_cancel_button) {
    this->close(2);
  }
  return true;
}

bool
WindowChild::handle_close()
{
  if (this->m_button_count) {
    this->close(this->m_button_count);
  }
  return true;
}

PG_Widget*
WindowChild::create_window()
{
  PG_Window *window = new PG_Window(NULL,
                                    PG_Rect(m_parent->my_xpos + (m_parent->my_width - m_width) / 2,
                                            m_parent->my_ypos + (m_parent->my_height - m_height) / 2,
                                            m_width,
                                            m_height),
                                    this->m_title.data(),
                                    this->m_close ? PG_Window::SHOW_CLOSE : PG_Window::MODAL);

  if (this->m_close)
    window->sigClose.connect(slot(*this, &WindowChild::handle_close));
    
  return window;
}
