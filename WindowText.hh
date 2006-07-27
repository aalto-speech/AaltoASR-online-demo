
#ifndef WINDOWTEXT_HH_
#define WINDOWTEXT_HH_

#include "WindowChild.hh"

class WindowText  :  public WindowChild
{

public:
  
  WindowText(const PG_Widget *parent, const std::string &title, const std::string &text)
    : WindowChild(parent, title, 450, 350, true)
  {
    this->m_text = text;
  }
  
  virtual ~WindowText() { };
  
  virtual void initialize()
  {
    Window::initialize();
    PG_RichEdit *text_area = new PG_RichEdit(this->m_window,
                                             PG_Rect(5,
                                                     30,
                                                     this->m_window->my_width - 10,
                                                     this->m_window->my_height - 35));
    text_area->SetTransparency(255);
    text_area->SetText(this->m_text);
    this->m_window->AddChild(text_area);
  }
  
private:
  
  std::string m_text;
};

#endif /*WINDOWTEXT_HH_*/
