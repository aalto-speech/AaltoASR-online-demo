
#ifndef WIDGETSTATUS_HH_
#define WIDGETSTATUS_HH_

#include "RecognizerStatus.hh"
#include <pgwidget.h>
#include <pglabel.h>

/** A status bar widget. Shows recognition status and adaptation status. */
class WidgetStatus  :  public PG_Widget
{
  
public:

  WidgetStatus(PG_Widget *parent,
               const PG_Rect &rect,
               const RecognizerStatus *recog_status);
  
  /** Updates the status texts. */
  void update();
  
private:

  const RecognizerStatus *m_recog_status; //!< Recognizer status.
  PG_Label *m_recognition_label;
  PG_Label *m_adaptation_label;
  
  // These variables are used to check the need for a update.
  RecognizerStatus::RecognitionStatus m_recognition_status;
  RecognizerStatus::AdaptationStatus m_adaptation_status;
  
};

#endif /*WIDGETSTATUS_HH_*/
