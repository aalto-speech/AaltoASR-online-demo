
#ifndef WIDGETRECOGNITIONTEXTS_HH_
#define WIDGETRECOGNITIONTEXTS_HH_

#include <vector>
#include "AudioInputController.hh"
#include "WidgetScrollArea.hh"
#include "RecognizerStatus.hh"

/** A scrollable view which shows the recognition text. It shows the morphemes
 * separately and wraps a single word into a box. It also forces word breaks.
 * Recognized and hypothesis parts are shown in different colors. */
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
  
  /** Removes all hypothesis widgets. */
  void remove_hypothesis();

  /** Creates a widget for a morpheme or a sentence break. Morphemes are
   * put into a word container. The logic between the return value and word
   * widget is as follows (if read after the function call!):
   * 1) ret == NULL && word == NULL  ==>  word break (no widget).
   * 2) ret != NULL && word == NULL  ==>  sentence break widget
   * 3) ret != NULL && word != NULL  ==>  morpheme widget.
   * \param morpheme The morpheme data.
   * \param color Text color.
   * \param word_widget Container widget of the current word. Don't modify it
   *                    yourself, just give the same variable every time and
   *                    the function will keep track.
   * \param min_x Minimum x coordinate for the word. Used to force word breaks.
   * \return The widget for the morpheme. */
  PG_Widget* add_morpheme_widget(const Morpheme &morpheme,
                                 const PG_Color &color,
                                 PG_Widget *&word_widget,
                                 Sint32 min_x);
  /** Requests playback when the morphemes are clicked. Callback function for
   * ParaGUI. */
  bool handle_morpheme_widget(PG_MessageObject *widget,
                              const SDL_MouseButtonEvent *event,
                              void *user_data);

private:

  const unsigned int m_pixels_per_second; //!< Time resolution.
  RecognizerStatus *m_recognition; //!< Source of the recognition.
  /** If a hypothesis morpheme is a part of a recognized word, it is stored
   * in this list as a morpheme. If a word is completely hypothesis it is stored
   * here as a word and the morphemes are not stored. This way we can always
   * destroy the hypothesis widgets and create new ones to make hypothesis
   * change in real time. */
  std::vector<PG_Widget*> m_hypothesis_widgets;
  /** Number of recognized (not hypothesis) morphemes. */
  unsigned int m_last_recognition_count;
  /** Used to check the recognition process has moved on since last time. */
  unsigned long m_last_recognition_frame;
  
  /** When morphemes are clicked the playback request is sent to this. */
  AudioInputController *m_audio_input;
  /** Last word which has recognized (not hypothesis) morphemes or NULL if it is
   * a word break. */
  PG_Widget *m_last_recog_word;
  /** Last recognized (not hypothesis) morpheme. */
  PG_Widget *m_last_recog_morph;
};

#endif /*WIDGETRECOGNITIONTEXTS_HH_*/
