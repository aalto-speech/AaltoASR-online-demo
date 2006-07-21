
#include "WindowMessageBox.hh"

WindowMessageBox::WindowMessageBox(const PG_Widget *parent,
                                   const std::string &title,
                                   const std::string &message,
                                   const std::string &button)
  : WindowChild(parent, title, 300, 200, false, button)
{
  this->m_message = message;
  this->m_text_label = NULL;
}

WindowMessageBox::WindowMessageBox(const PG_Widget *parent,
                                   const std::string &title,
                                   const std::string &message,
                                   const std::string &button1,
                                   const std::string &button2)
  : WindowChild(parent, title, 300, 200, false, button1, button2)
{
  this->m_message = message;
  this->m_text_label = NULL;
}

WindowMessageBox::~WindowMessageBox()
{
  delete this->m_text_label;
}

void
WindowMessageBox::initialize()
{
  WindowChild::initialize();

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
