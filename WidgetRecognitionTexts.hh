
#ifndef WIDGETRECOGNITIONTEXTS_HH_
#define WIDGETRECOGNITIONTEXTS_HH_

#include <pgscrollarea.h>
#include <pglabel.h>
#include <pgbutton.h>
#include <vector>
#include "WidgetScrollArea.hh"
#include "Recognition.hh"

class WidgetRecognitionTexts  :  public WidgetScrollArea
{
  
public:

  WidgetRecognitionTexts(PG_Widget *parent, const PG_Rect &rect,
                         Recognition *recognition, unsigned int pixels_per_second);
  virtual ~WidgetRecognitionTexts();
  
  void update();
  void reset();
  
protected:

  void initialize();
  void terminate();

  void add_morpheme_button(const Morpheme *morpheme);

  const unsigned int m_pixels_per_second;
  Recognition *m_recognition;
  std::vector<PG_Label*> m_morpheme_buttons;
  unsigned int m_last_morpheme_count;
};

#endif /*WIDGETRECOGNITIONTEXTS_HH_*/
