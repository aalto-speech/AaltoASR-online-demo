
#ifndef WIDGETTIMEAXIS_HH_
#define WIDGETTIMEAXIS_HH_

#include "WidgetScrollArea.hh"

/** A widget to show the time axis in the scrollable view. */
class WidgetTimeAxis  :  public WidgetScrollArea
{

public:

  WidgetTimeAxis(PG_Widget *parent,
                 const PG_Rect &rect,
                 unsigned int pixels_per_second);
  virtual ~WidgetTimeAxis() { }
  
  /** Removes all time ticks. */
  void reset();
  /** Updates the time axis. Only adds new time ticks, doesn't remove.
   * \param time_length Current length of the time axis. */
  void update(unsigned int time_length);
  
protected:

  /** Adds a time tick.
   * \param time Time to add. */
  void add_time_label(unsigned int time);
  
private:

  const unsigned int m_pixels_per_second; //!< Time resolution.
  unsigned int m_last_time; //!< Time length in last update.

};

#endif /*WIDGETTIMEAXIS_HH_*/
