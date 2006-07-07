
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
//  fprintf(stderr, "Beginning of WSA:set_scroll_position\n");
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
//      fprintf(stderr, "Scrolling widget... x:%d, y:%d\n", iter->first - x, iter->second->my_ypos);
      widget->my_xpos = iter->first - x;
      widget->Show(false);
//      fprintf(stderr, "Widget scrolled.\n", iter->first - x, iter->second->my_ypos);
    }
    else {
      widget->Hide(false);
    }
  }
  this->m_scroll_x = x;
  this->Update(true);
//  this->Redraw(true);
  /*
  for (iter = this->m_items.begin();
       iter != this->m_items.end();
       iter++)
  {
    // Just move the widget aside.
//    iter->second->MoveWidget(-iter->second->my_width, iter->second->my_ypos);
    iter->second->Hide(false);
  }
  
  // Näyttää toistaiseksi vain ne, joiden vasen reuna osuu alueeseen.
  if (x != this->m_scroll_x) {
    for (iter = this->m_items.lower_bound(x);
         iter != this->m_items.upper_bound(x+this->my_width);
         iter++)
    {
      iter->second->MoveWidget(iter->first - x, iter->second->my_ypos - this->my_ypos);
//      iter->second->SetVisible(true);
      iter->second->Show(false);
//      iter->second->Update(true);
//      iter->second->Redraw(true);
    }
  }
  this->m_scroll_x = x;
//  this->Update(true);
//  this->Redraw(true);
  //*/
  
//  fprintf(stderr, "end of WSA:set_scroll_position\n");
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
    item->Show(false);
  }
  else {
    item->Hide(false);
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
