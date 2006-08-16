
#ifndef WIDGETTEXTSAREA_HH_
#define WIDGETTEXTSAREA_HH_

#include <pgwidget.h>
#include "WidgetMultiLineEdit.hh"
#include "RecognitionParser.hh"
#include "Window.hh"

class WidgetComparisonArea  :  public PG_Widget
{

public:

  WidgetComparisonArea(Window &parent,
                       const PG_Rect &rect,
                       RecognitionParser *recognition);
  virtual ~WidgetComparisonArea() { }
  
protected:

  bool handle_openoriginal_button();
  bool handle_saveoriginal_button();
  bool handle_clearoriginal_button();
  bool handle_editoriginal_button();
  bool handle_updaterecognition_button();
  bool handle_saverecognition_button();
  bool handle_compare_button();

private:

  Window &m_parent;
  RecognitionParser *m_recognition;
  WidgetMultiLineEdit *m_original_text;
  WidgetMultiLineEdit *m_recognition_text;
  
};

#endif /*WIDGETTEXTSAREA_HH_*/
