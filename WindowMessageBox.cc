
#include "WindowMessageBox.hh"

WindowMessageBox::WindowMessageBox(const PG_Widget *parent, const std::string &title, const std::string &message, const std::string &button)
  : WindowChild(parent, title, 300, 200, false, button)
{
  /*
  this->m_title = title;
  this->m_button1_text = button;
  this->m_two_buttons = false;
  this->m_button1 = NULL;
  this->m_button2 = NULL;
//*/
  this->m_message = message;
  this->m_text_label = NULL;
}

WindowMessageBox::WindowMessageBox(const PG_Widget *parent, const std::string &title, const std::string &message, const std::string &button1, const std::string &button2)
  : WindowChild(parent, title, 300, 200, false, button1, button2)
{
  /*
  this->m_title = title;
  this->m_button1_text = button1;
  this->m_button2_text = button2;
  this->m_two_buttons = true;
  this->m_button1 = NULL;
  this->m_button2 = NULL;
//*/
  this->m_message = message;
  this->m_text_label = NULL;
}
/*
PG_Widget*
WindowMessageBox::create_window()
{
  return new PG_Window(NULL,
                       PG_Rect(m_parent->my_xpos + (m_parent->my_width - 300) / 2,
                               m_parent->my_ypos + (m_parent->my_height - 200) / 2,
                               300,
                               200),
                       this->m_title.data(),
                       PG_Window::MODAL);

}
//*/
WindowMessageBox::~WindowMessageBox()
{
  /*
  delete this->m_button1;
  delete this->m_button2;
  //*/
  delete this->m_text_label;
}

void
WindowMessageBox::initialize()
{
  WindowChild::initialize();
  /*
  PG_Rect rect1, rect2;
  
  if (!this->m_two_buttons) {
    rect1.SetRect(this->m_window->my_width / 2 - 30,
                  this->m_window->my_height - 40,
                  60,
                  30);
  }
  else {
    rect1.SetRect(this->m_window->my_width / 2 - 70,
                  this->m_window->my_height - 40,
                  60,
                  30);

    rect2.SetRect(this->m_window->my_width / 2 + 10,
                  this->m_window->my_height - 40,
                  60,
                  30);
  }
  
  this->m_button1 = new PG_Button(this->m_window, rect1, this->m_button1_text.data());
  this->m_button1->sigClick.connect(slot(*this, &WindowMessageBox::handle_button));
  this->m_window->AddChild(this->m_button1);

  if (this->m_two_buttons) {
    this->m_button2 = new PG_Button(this->m_window, rect2, this->m_button2_text.data());
    this->m_button2->sigClick.connect(slot(*this, &WindowMessageBox::handle_button));
    this->m_window->AddChild(this->m_button2);
  }
  
  //*/
  this->m_text_label = new PG_RichEdit(this->m_window,
                                       PG_Rect(10,
                                               40,
                                               this->m_window->my_width - 20,
                                               this->m_window->my_height - 90));

  this->m_text_label->SendToBack();
  this->m_text_label->SetTransparency(255);
  this->m_text_label->SetText(this->m_message.data());
  this->m_window->AddChild(this->m_text_label);

}
/*
bool
WindowMessageBox::handle_button(PG_Button *button)
{
  if (button == this->m_button1) {
    this->close(1);
  }
  if (button == this->m_button2) {
    this->close(2);
  }
  return true;
}
//*/
