
#ifndef WIDGETRECOGNITIONTEXTS_HH_
#define WIDGETRECOGNITIONTEXTS_HH_

#include <pgscrollarea.h>
#include <pglabel.h>
#include <pgbutton.h>
#include <vector>
#include "AudioInputController.hh"
#include "WidgetScrollArea.hh"
#include "Recognition.hh"

class WidgetRecognitionTexts  :  public WidgetScrollArea
{
  
public:

  WidgetRecognitionTexts(PG_Widget *parent, const PG_Rect &rect,
                         AudioInputController *audio_input,
                         Recognition *recognition, unsigned int pixels_per_second);
  virtual ~WidgetRecognitionTexts();
  
  void update();
  void reset();
  
protected:

  void initialize();
  void terminate();

  void add_morpheme_button(const Morpheme *morpheme);
  void remove_morpheme_button(unsigned int index);
  bool handle_morpheme_button(PG_MessageObject *widget, const SDL_MouseButtonEvent *event, void *user_data);

  const unsigned int m_pixels_per_second;
  Recognition *m_recognition;
  std::vector<PG_Widget*> m_morpheme_buttons;
  unsigned int m_last_morpheme_count;
  
  AudioInputController *m_audio_input;
};

#endif /*WIDGETRECOGNITIONTEXTS_HH_*/
