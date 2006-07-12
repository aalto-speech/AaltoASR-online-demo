
#include "WidgetScrollArea.hh"

WidgetScrollArea::WidgetScrollArea(PG_Widget *parent, const PG_Rect &rect)
  : PG_Widget(parent, rect, false)
{
  this->m_scroll_x = 0;
}

WidgetScrollArea::~WidgetScrollArea()
{
  
}
  
void
WidgetScrollArea::set_scroll_position(Sint32 x)
{
  MapScroll::iterator iter;
  PG_Widget *widget;

  // Ainakin alkuun käydään vaan karusti kaikki läpi..
  for (iter = this->m_items.begin();
       iter != this->m_items.end();
       iter++)
  {
    widget = iter->second;
    if (iter->first < x + this->my_width &&
        iter->first + widget->my_width > x)
    {
      widget->my_xpos = iter->first - x;
      widget->SetVisible(true);//Show(false);
    }
    else {
      widget->SetVisible(false);//Hide(false);
    }
  }
  this->m_scroll_x = x;
//  this->Update(true);
}
  
void
WidgetScrollArea::add_child(PG_Widget *item, Sint32 x)
{
  this->AddChild(item);
  this->m_items.insert(std::make_pair(x, item));
  if (x < this->m_scroll_x + this->my_width &&
      x + item->my_width > this->m_scroll_x)
  {
    item->my_xpos = x - this->m_scroll_x;
    item->SetVisible(true);
  }
  else {
    item->SetVisible(false);
  }
}
  
void
WidgetScrollArea::remove_child(const PG_Widget *item)
{
  
}

void
WidgetScrollArea::remove_all_childs()
{
  this->m_items.clear();
  this->RemoveAllChilds();
}
