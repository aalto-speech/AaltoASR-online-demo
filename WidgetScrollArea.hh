
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

  /** 
   * \param parent Parent widget.
   * \param rect Rectangle area of the widget. */
  WidgetScrollArea(PG_Widget *parent, const PG_Rect &rect);
  virtual ~WidgetScrollArea() { }
  
  /** \param x The scroll position of the widget. */
  void set_scroll_position(Sint32 x);
  /** \return The scroll position of the widget. */
  inline Sint32 get_scroll_position() const;
  
  /** Overwrites the default AddChild function. Reads the desired x coordinate
   * from user data, therefore the items user data MUST be set before adding
   * it as a child to this widget.
   * \param item Child widget to add. */
  virtual void AddChild(PG_Widget *item);

protected:

  /** Calculates the positions of the widgets in the view according to scroll
   * position normally. Exception: when the item is far away from the view and
   * 2 byte integers might get messy, it is just set aside to constant distance.
   * \param item Widget to position. */
  void set_widget_position(PG_Widget *item) const;
  
private:

  Sint32 m_scroll_x; //!< X position of the scroll.

};

Sint32
WidgetScrollArea::get_scroll_position() const
{
  return this->m_scroll_x;
}

#endif /*WIDGETSCROLLAREA_HH_*/
