//*
#include <assert.h>
#include "WidgetScrollArea.hh"

WidgetScrollArea::WidgetScrollArea(PG_Widget *parent, const PG_Rect &rect)
  : PG_Widget(parent, rect, false)
{
  this->m_scroll_x = 0;
}

void
WidgetScrollArea::set_scroll_position(Sint32 xscroll)
{
  PG_RectList *child_list = this->GetChildList();
  
  this->m_scroll_x = xscroll;

  if (child_list != NULL) {
    PG_Widget *item = child_list->first();
    unsigned int index;
  
    // Ainakin alkuun k‰yd‰‰n vaan karusti kaikki l‰pi..
    for (index = 0; index < child_list->size(); index++)
    {
      this->set_widget_position(item);
      item = item->next();
    }
  }

}

void
WidgetScrollArea::set_widget_position(PG_Widget *item) const
{
  Sint32 x;
  item->GetUserData(&x);
  if (x < this->m_scroll_x + this->my_width &&
      x + item->my_width > this->m_scroll_x)
  {
    item->my_xpos = x - this->m_scroll_x;
  }
  else {
    item->my_xpos = -item->my_width;
  }
}

void
WidgetScrollArea::AddChild(PG_Widget *item)
{
  PG_Widget::AddChild(item);
  this->set_widget_position(item);
}
