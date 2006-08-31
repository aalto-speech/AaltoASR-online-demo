
#ifndef WIDGETTEXTSAREA_HH_
#define WIDGETTEXTSAREA_HH_

#include <pgwidget.h>
#include "WidgetMultiLineEdit.hh"
#include "RecognizerStatus.hh"
#include "Window.hh"

/** A GUI class which contains the two text fields (reference and hypothesis)
 * and several buttons for handling them. */
class WidgetComparisonArea  :  public PG_Widget
{

public:

  WidgetComparisonArea(Window &parent,
                       const PG_Rect &rect,
                       RecognizerStatus *recognition);
  virtual ~WidgetComparisonArea();
  
protected:

  /** Callback functions for ParaGUI. */
  bool handle_openoriginal_button();
  bool handle_saveoriginal_button();
  bool handle_clearoriginal_button();
  bool handle_editoriginal_button();
  bool handle_pasteoriginal_button();
  bool handle_updaterecognition_button();
  bool handle_saverecognition_button();
  bool handle_compare_button();

private:

  Window &m_parent; //!< Parent window. Needed for running child windows.
  RecognizerStatus *m_recognition; //!< Source for recognition.
  WidgetMultiLineEdit *m_original_text; //!< Text field for reference.
  WidgetMultiLineEdit *m_recognition_text; //!< Text field for hypothesis.
  
};

#endif /*WIDGETTEXTSAREA_HH_*/
