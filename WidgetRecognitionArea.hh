
#ifndef WIDGETRECOGNITIONAREA_HH_
#define WIDGETRECOGNITIONAREA_HH_

#include <pgradiobutton.h>
#include "WidgetScrollBar.hh"
#include "RecognizerStatus.hh"
#include "AudioInputController.hh"
#include "WidgetRecognitionText.hh"
#include "WidgetWave.hh"
#include "WidgetSpectrogram.hh"
#include "WidgetTimeAxis.hh"

/** Forms an area that includes wave and spectrogram views of the audio,
 * recognition text, time axis, auto scrolling radio buttons and a scroll
 * bar. */
class WidgetRecognitionArea  :  public PG_Widget
{
  
public:

  /** Constructs the widget.
   * \param parent Parent widget.
   * \param rect Rectangle area of the widget.
   * \param audio_input Source of audio data.
   * \param recognition Source of recognition information.
   * \param pixels_per_second Time resolution used for all children. */
  WidgetRecognitionArea(PG_Widget *parent,
                        const PG_Rect &rect,
                        AudioInputController *audio_input,
                        RecognizerStatus *recognition,
                        unsigned int pixels_per_second);
  virtual ~WidgetRecognitionArea();

  /** Resets all child widgets and updates the scroll position. */
  void reset();
  /** Updates all child widgets. Should be called as often as possible to get
   * fast graphics. */
  void update();
  
  /** Sets the scroll position of all childs.
   * \param page Scroll position. */
  void set_scroll_position(unsigned long page);
  
  /** \return The spectrogram child widget. */
  inline WidgetSpectrogram* get_spectrogram();

protected:
  
  /** Callback functions for ParaGUI. */
  bool handle_scroll(PG_ScrollBar *scroll_bar, long page);
  bool handle_radio(PG_RadioButton *radio, bool status, void *user_data);
  
  /** Updates the scroll bar by checking the size of the audio data. */
  void set_scroll_range();
  
  void update_cursors();
  void draw_cursor(long position, PG_Color color);
  
  inline unsigned long get_audio_cursor() const;
  inline unsigned long get_recognizer_cursor() const;
  inline unsigned long get_playback_cursor() const;
  inline unsigned long get_audio_pixels() const;
  
private:

  /** Updates the screen areas of the child widgets.
   * \param new_data true if new recognitions should be checked.
   * TODO: The parameter has a very little affect and it could be removed. */
  void update_screen(bool new_data);

  // Some widgets that form this area.
  WidgetWave *m_wave; //!< Wave view widget.
  WidgetSpectrogram *m_spectrogram; //!< Spectrogram view widget.
  WidgetRecognitionText *m_text_area; //!< Recognition text widget.
  WidgetTimeAxis *m_time_axis; //!< Time axis widget.
  WidgetScrollBar *m_scroll_bar; //!< Horizontal scroll bar.
  PG_RadioButton *m_disablescroll_radio; //!< Disable auto scroll radio.

  AudioInputController *m_audio_input; //!< Audio input controller.
  RecognizerStatus *m_recognition; //!< Recognition texts.

  const unsigned int m_pixels_per_second; //!< Scale of the view.
  const double m_frames_per_pixel; //!< Audio samples per one pixel.

  enum Autoscroll { DISABLE, RECOGNIZER, AUDIO };
  Autoscroll m_autoscroll; //!< Auto scrolling status.
  
};

WidgetSpectrogram*
WidgetRecognitionArea::get_spectrogram()
{
  return this->m_spectrogram;
}

unsigned long
WidgetRecognitionArea::get_audio_cursor() const
{
  return (unsigned long)(this->m_audio_input->get_audio_cursor() /
                         this->m_frames_per_pixel);
}

unsigned long
WidgetRecognitionArea::get_recognizer_cursor() const
{
  if (m_recognition->m_message_result_true_called)
    return get_audio_pixels();
  else
    return (unsigned long)(this->m_pixels_per_second *
                           this->m_recognition->get_recognition_frame() /
                           RecognizerStatus::frames_per_second);
}

unsigned long 
WidgetRecognitionArea::get_playback_cursor() const
{
    return (unsigned long)(this->m_audio_input->get_playback_cursor() /
                           this->m_frames_per_pixel);
}

unsigned long
WidgetRecognitionArea::get_audio_pixels() const
{
  return (unsigned long)(this->m_audio_input->get_audio_data_size() /
                         this->m_frames_per_pixel);
}

#endif /*WIDGETRECOGNITIONAREA_HH_*/
