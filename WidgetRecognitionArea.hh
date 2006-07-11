
#ifndef WIDGETRECOGNITIONAREA_HH_
#define WIDGETRECOGNITIONAREA_HH_

#include <pgscrollbar.h>
#include "Recognition.hh"
#include "AudioInputController.hh"
#include "WidgetRecognitionTexts.hh"
#include "WidgetWave.hh"
#include "WidgetSpectrogram.hh"

class WidgetRecognitionArea  :  public PG_Widget
{
  
public:

  WidgetRecognitionArea(PG_Widget *parent,
                        const PG_Rect &rect,
                        AudioInputController *audio_input,
                        Recognition *recognition,
                        unsigned int pixels_per_second);
  virtual ~WidgetRecognitionArea();

  void reset();
  void update();
  
  void set_autoscroll(bool autoscroll) { this->m_autoscroll = autoscroll; };
  
protected:
  
  bool handle_scroll(PG_ScrollBar *scroll_bar, long page);
  
  void set_scroll_range();
  
//  void initialize();
//  void terminate();
  
private:

  void set_scroll_position(unsigned long page);

  WidgetWave *m_wave;
  WidgetSpectrogram *m_spectrogram;
  WidgetRecognitionTexts *m_text_area;
  PG_ScrollBar *m_scroll_bar;
  AudioInputController *m_audio_input;
  const unsigned int m_pixels_per_second;
  const unsigned int m_frames_per_pixel;
  bool m_autoscroll;
  pthread_mutex_t m_scroll_lock;
  
};

#endif /*WIDGETRECOGNITIONAREA_HH_*/
