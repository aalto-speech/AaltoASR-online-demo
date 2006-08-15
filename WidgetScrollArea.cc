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
  //*
  if (abs(x - this->m_scroll_x) < 20000)
    item->MoveWidget(x - this->m_scroll_x, 0, false);
  else
    item->MoveWidget(-20000, 0, false);
    //*/
  /*
  if (x < this->m_scroll_x + this->my_width &&
      x + item->my_width > this->m_scroll_x)
  {
    item->MoveWidget(x - this->m_scroll_x, 0, false);
    //item->my_xpos = x - this->m_scroll_x;
    item->SetVisible(true);
  }
  else {
    //item->MoveWidget(-10000, 0, false);
    //item->my_xpos = -item->my_width;
    item->SetVisible(false);
  }
  //*/
}

void
WidgetScrollArea::AddChild(PG_Widget *item)
{
//  fprintf(stderr, "add child 1\n");
  item->SetVisible(false);
//  fprintf(stderr, "add child 2\n");
  PG_Widget::AddChild(item);
//  fprintf(stderr, "add child 3\n");
  this->set_widget_position(item);
//  fprintf(stderr, "add child 4\n");
}
