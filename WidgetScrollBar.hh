
#ifndef WIDGETSCROLLBAR_HH_
#define WIDGETSCROLLBAR_HH_

#include <pgscrollbar.h>

class WidgetScrollBar  :  public PG_ScrollBar
{
  
public:

  WidgetScrollBar(PG_Widget *parent,
                  const PG_Rect &rect = PG_Rect::null,
                  ScrollDirection direction = VERTICAL,
                  int id = -1,
                  const std::string &style = "Scrollbar");
  virtual ~WidgetScrollBar() { }

  /** This will overwrite the function in a STATIC way. */  
  void SetRange(Uint32 min, Uint32 max);

protected:

  virtual void RecalcPositions();
  
private:
  
//  void calculate_scrollbutton_positions(Uint16 length, Uint16 thickness);
  void calculate_position_and_size(Sint16 start_pos,
                                   Uint16 total_size,
                                   Sint16 &position,
                                   Uint16 &size);
                                   
  void calculate_borders(Sint16 &position, Uint16 &size);
  
};

#endif /*WIDGETSCROLLBAR_HH_*/
