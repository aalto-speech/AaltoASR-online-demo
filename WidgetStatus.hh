
#ifndef WIDGETSTATUS_HH_
#define WIDGETSTATUS_HH_

#include "RecognizerStatus.hh"
#include <pgwidget.h>
#include <pglabel.h>

class WidgetStatus  :  public PG_Widget
{
  
public:

  WidgetStatus(PG_Widget *parent,
               const PG_Rect &rect,
               const RecognizerStatus *recog_status);
  
  void update();
  
private:

  const RecognizerStatus *m_recog_status;
  PG_Label *m_recognition_label;
  PG_Label *m_adaptation_label;
  
};

#endif /*WIDGETSTATUS_HH_*/
