
#include "WindowMessageBox.hh"
#include <pgrichedit.h>

WindowMessageBox::WindowMessageBox(const PG_Widget *parent,
                                   const std::string &title,
                                   const std::string &message,
                                   const std::string &button)
  : WindowChild(parent, title, 300, 200, false, true, button)
{
  this->m_message = message;
}

WindowMessageBox::WindowMessageBox(const PG_Widget *parent,
                                   const std::string &title,
                                   const std::string &message,
                                   const std::string &button1,
                                   const std::string &button2)
  : WindowChild(parent, title, 300, 200, false, true, button1, button2)
{
  this->m_message = message;
}

void
WindowMessageBox::initialize()
{
  WindowChild::initialize();

  PG_RichEdit *text_label = new PG_RichEdit(this->m_window,
                                            PG_Rect(10,
                                                    40,
                                                    this->m_window->my_width - 20,
                                                    this->m_window->my_height - 90));

  text_label->SendToBack();
  text_label->SetTransparency(255);
  text_label->SetText(this->m_message.data());
  this->m_window->AddChild(text_label);

}
