
#ifndef WIDGETSCROLLAREA_HH_
#define WIDGETSCROLLAREA_HH_

#include <pgscrollbar.h>
#include "WidgetSpectrum.hh"

class WidgetRecognitionArea  :  public PG_Widget
{
  
public:

  WidgetRecognitionArea(PG_Widget *parent, const PG_Rect &rect);
  ~WidgetRecognitionArea();
  
private:

  WidgetSpectrum *m_spectrum;
  PG_ScrollBar *m_scroll_bar;
  
};

#endif /*WIDGETSCROLLAREA_HH_*/
