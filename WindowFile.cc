
#include <assert.h>
#include "WindowFile.hh"

// This is to fasten debugging..
std::string WindowFile::last_file = "chunk.wav";

WindowFile::WindowFile(const PG_Widget *parent, const std::string &title)
  : WindowChild(parent, title, 400, 200, true, "OK", "Cancel")
{
  this->m_filename_textbox = NULL;
  this->m_text_label = NULL;
  /*
  this->m_ok_button = NULL;
  this->m_cancel_button = NULL;
  this->m_ok_pressed = false;
  //*/
}

WindowFile::~WindowFile()
{
  delete this->m_filename_textbox;
  delete this->m_text_label;
  /*
  delete this->m_ok_button;
  delete this->m_cancel_button;
  //*/
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
/*
  this->m_ok_button = new PG_Button(this->m_window,
                                    PG_Rect(this->m_window->my_width / 2 - 70,
                                            this->m_window->my_height - 40,
                                            60,
                                            30),
                                    "OK");
  this->m_cancel_button = new PG_Button(this->m_window,
                                        PG_Rect(this->m_window->my_width / 2 + 10,
                                                this->m_window->my_height - 40,
                                                60,
                                                30),
                                        "Cancel");
  this->m_window->AddChild(this->m_ok_button);
  this->m_window->AddChild(this->m_cancel_button);
  this->m_ok_button->sigClick.connect(slot(*this, &WindowFile::handle_button));
  this->m_cancel_button->sigClick.connect(slot(*this, &WindowFile::handle_button));
//*/  
  this->m_window->AddChild(this->m_text_label);
  this->m_window->AddChild(this->m_filename_textbox);
  
  this->m_filename_textbox->SetText(WindowFile::last_file.data());
}
/*
void
WindowFile::do_running()
{
  if (this->m_ok_pressed) {
    this->handle_ok_pressed();
    this->m_ok_pressed = false;
  }
}
//*/
void
WindowFile::do_closing(int return_value)
{
  if (return_value == 1) {
    WindowFile::last_file = this->get_filename();
  }
}

/*
PG_Widget*
WindowFile::create_window()
{
  if (this->m_parent == NULL) {
    fprintf(stderr, "WindowOpenFile::create_window got NULL-parent.\n");
    assert(false);
  }

  Uint16 width = 400;
  Uint16 height = 200;
  Sint16 x = this->m_parent->my_xpos + (this->m_parent->my_width - width) / 2;
  Sint16 y = this->m_parent->my_ypos + (this->m_parent->my_height - height) / 2;

  PG_Window *window = new PG_Window(NULL,
                                    PG_Rect(x, y, width, height),
                                    this->get_title().data(),
                                    PG_Window::SHOW_CLOSE);

  window->sigClose.connect(slot(*this, &WindowFile::handle_close));
  
  return window;
  
}
//*/
/*
bool
WindowFile::handle_button(PG_Button *button)
{
  if (button == this->m_ok_button) {
    this->m_ok_pressed = true;
  }
  else if (button == this->m_cancel_button) {
    this->close(-1);
  }
  return true;
}
//*/
/*
bool
WindowFile::handle_close()
{
  this->close(-1);
  return true;
}
//*/