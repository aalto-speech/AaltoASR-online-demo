
#ifndef WIDGETSCROLLAREA_HH_
#define WIDGETSCROLLAREA_HH_

#include <pgwidget.h>
#include <map>

// TODO: Muutetaanko periytymissuhde omistussuhteeksi?
class WidgetScrollArea  :  public PG_Widget
{
  
public:

  WidgetScrollArea(PG_Widget *parent, const PG_Rect &rect);
  virtual ~WidgetScrollArea();
  
  void set_scroll_position(Sint32 x);
  inline Sint32 get_scroll_position() const { return this->m_scroll_x; }
  
  // Use this instead of PG_Widget::AddChild if you want the item to be
  // scrolled ! ! !
  void add_child(PG_Widget *item, Sint32 x);
  void remove_child(const PG_Widget *item);
  void remove_all_childs();
  
private:

  PG_Widget::AddChild;
  PG_Widget::RemoveChild;
  PG_Widget::RemoveAllChilds;

  typedef std::multimap<Sint32, PG_Widget*> MapScroll;

  Sint32 m_scroll_x;
  MapScroll m_items;

};

#endif /*WIDGETSCROLLAREA_HH_*/
