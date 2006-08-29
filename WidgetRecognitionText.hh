
#ifndef WIDGETRECOGNITIONTEXTS_HH_
#define WIDGETRECOGNITIONTEXTS_HH_

#include <vector>
#include "AudioInputController.hh"
#include "WidgetScrollArea.hh"
#include "RecognizerStatus.hh"

class WidgetRecognitionText  :  public WidgetScrollArea
{
  
public:

  WidgetRecognitionText(PG_Widget *parent,
                         const PG_Rect &rect,
                         AudioInputController *audio_input,
                         RecognizerStatus *recognition,
                         unsigned int pixels_per_second);
  virtual ~WidgetRecognitionText();
  
  void update();
  void reset();
  
protected:

  void initialize();
  void terminate();

  void update_recognition();
  void update_hypothesis();
  
  void remove_hypothesis();

  PG_Widget* add_morpheme_widget(const Morpheme &morpheme,
                                 const PG_Color &color,
                                 PG_Widget *&word_widget,
                                 Sint32 min_x);
  bool handle_morpheme_widget(PG_MessageObject *widget,
                              const SDL_MouseButtonEvent *event,
                              void *user_data);

private:

  const unsigned int m_pixels_per_second;
  RecognizerStatus *m_recognition;
  std::vector<PG_Widget*> m_hypothesis_widgets;
  unsigned int m_last_recognition_count;
  unsigned long m_last_recognition_frame;
//  MorphemeList::const_iterator m_recognized_iter;
//  const Morpheme *m_last_recognized_morpheme;
  
  AudioInputController *m_audio_input;
  PG_Widget *m_last_recog_word;
  PG_Widget *m_last_recog_morph;
};

#endif /*WIDGETRECOGNITIONTEXTS_HH_*/
