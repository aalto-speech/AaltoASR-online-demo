
#ifndef WIDGETTIMEAXIS_HH_
#define WIDGETTIMEAXIS_HH_

#include "WidgetScrollArea.hh"

class WidgetTimeAxis  :  public WidgetScrollArea
{

public:

  WidgetTimeAxis(PG_Widget *parent,
                 const PG_Rect &rect,
                 unsigned int pixels_per_second);
  virtual ~WidgetTimeAxis() { }
  
  void reset();
  void update(unsigned int time_length);
  
protected:

  void add_time_label(unsigned int time);
  
private:

  const unsigned int m_pixels_per_second;
  unsigned int m_last_time;

};

#endif /*WIDGETTIMEAXIS_HH_*/
