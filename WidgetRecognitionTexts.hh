
#ifndef WIDGETRECOGNITIONTEXTS_HH_
#define WIDGETRECOGNITIONTEXTS_HH_

#include "undefine.hh"
#include <pglabel.h>
#include <pgbutton.h>
#include <vector>
#include "AudioInputController.hh"
#include "WidgetScrollArea.hh"
//#include <pgscrollarea.h>
#include "Recognition.hh"

class WidgetRecognitionTexts  :  public WidgetScrollArea//PG_ScrollArea//
{
  
public:

  WidgetRecognitionTexts(PG_Widget *parent,
                         const PG_Rect &rect,
                         AudioInputController *audio_input,
                         Recognition *recognition,
                         unsigned int pixels_per_second);
  virtual ~WidgetRecognitionTexts();
  
  void update();
  void reset();
  
protected:

  void initialize();
  void terminate();

  void update_recognition();
  void update_hypothesis();
  
  void remove_hypothesis();

  PG_Widget* add_morpheme_widget(const Morpheme &morpheme, const PG_Color &color);
  bool handle_morpheme_widget(PG_MessageObject *widget,
                              const SDL_MouseButtonEvent *event,
                              void *user_data);

private:

  const unsigned int m_pixels_per_second;
  Recognition *m_recognition;
  std::vector<PG_Widget*> m_hypothesis_widgets;
  unsigned int m_last_recognition_count;
  
  AudioInputController *m_audio_input;
};

#endif /*WIDGETRECOGNITIONTEXTS_HH_*/
