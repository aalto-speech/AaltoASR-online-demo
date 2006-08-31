
/** 
 * Some comments here:
 * ParaGUI's own PG_ScrollArea has so much disadvantages and weird behaviours,
 * that I rather make a scrolling area of my own. You should also note that
 * because PG_Rect uses Sint16 variables we have to use PG_Widget's user data
 * variable to store the real x-coordinate in 4 bytes. Otherwise the
 * x-coordinate range is insufficient for our purpose. The user data MUST be
 * set before adding a widget as a child into this widget!
 */

#ifndef WIDGETSCROLLAREA_HH_
#define WIDGETSCROLLAREA_HH_

#include <pgwidget.h>

class WidgetScrollArea  :  public PG_Widget
{
  
public:

  WidgetScrollArea(PG_Widget *parent, const PG_Rect &rect);
  virtual ~WidgetScrollArea() { }
  
  void set_scroll_position(Sint32 x);
  inline Sint32 get_scroll_position() const { return this->m_scroll_x; }
  
  virtual void AddChild(PG_Widget *item);

protected:

  void set_widget_position(PG_Widget *item) const;
  
private:

  Sint32 m_scroll_x;

};

#endif /*WIDGETSCROLLAREA_HH_*/
