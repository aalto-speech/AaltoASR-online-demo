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
  
    // Set widget positions.
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
  if (abs(x - this->m_scroll_x) < 20000) {
    // If the widget is close to or in the view, put it in correct coordinates -
    // although it might not be in the visible view.
    item->MoveWidget(x - this->m_scroll_x, 0, false);
  }
  else {
    // If the widget is far from the view, put it constant distance. We cannot
    // put it mathematically correct position, because the PG_Rect uses
    // 2 byte integer for the coordinate.
    item->MoveWidget(-20000, 0, false);
  }
}

void
WidgetScrollArea::AddChild(PG_Widget *item)
{
  item->SetVisible(false);
  PG_Widget::AddChild(item);
  this->set_widget_position(item);
}
