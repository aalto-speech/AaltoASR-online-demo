
#ifndef WIDGETCHECKBUTTON_HH_
#define WIDGETCHECKBUTTON_HH_

#include <pgcheckbutton.h>

class WidgetCheckButton  :  public PG_CheckButton
{
  
public:
  
  WidgetCheckButton(PG_Widget *parent, const PG_Rect &rect, const std::string &label)
    : PG_CheckButton(parent, rect, label.c_str()) { }
  virtual ~WidgetCheckButton()  { }
  
protected:

  virtual bool eventMouseButtonUp(const SDL_MouseButtonEvent *button) {
    fprintf(stderr, "WCheck, click\n");
    if (this->GetPressed()) {
      fprintf(stderr, "WCheck, to unpress\n");
      this->SetUnpressed();
    }
    else 
      this->SetPressed();
    return true;
  }
};

#endif /*WIDGETCHECKBUTTON_HH_*/
