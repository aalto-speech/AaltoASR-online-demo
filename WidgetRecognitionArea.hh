
#ifndef WIDGETRECOGNITIONAREA_HH_
#define WIDGETRECOGNITIONAREA_HH_

#include <pgradiobutton.h>
#include "WidgetScrollBar.hh"
//#include <pgscrollbar.h>
#include "RecognitionParser.hh"
#include "AudioInputController.hh"
#include "WidgetRecognitionTexts.hh"
#include "WidgetWave.hh"
#include "WidgetSpectrogram.hh"
#include "WidgetTimeAxis.hh"

class WidgetRecognitionArea  :  public PG_Widget
{
  
public:

  enum Autoscroll { DISABLE, RECOGNIZER, AUDIO };

  WidgetRecognitionArea(PG_Widget *parent,
                        const PG_Rect &rect,
                        AudioInputController *audio_input,
                        RecognitionParser *recognition,
                        unsigned int pixels_per_second);
  virtual ~WidgetRecognitionArea();

  void reset();
  void update();
  
  void set_scroll_position(unsigned long page);

protected:
  
  bool handle_scroll(PG_ScrollBar *scroll_bar, long page);
  bool handle_radio(PG_RadioButton *radio, bool status, void *user_data);
  
  void set_scroll_range();
  
  void update_cursors();
  void draw_cursor(long position, PG_Color color);
  
private:

  void update_screen(bool new_data);

  WidgetWave *m_wave;
  WidgetSpectrogram *m_spectrogram;
  WidgetRecognitionTexts *m_text_area;
  WidgetTimeAxis *m_time_axis;
  WidgetScrollBar *m_scroll_bar;
  PG_RadioButton *m_disablescroll_radio;

  AudioInputController *m_audio_input;
  RecognitionParser *m_recognition;

  const unsigned int m_pixels_per_second;
  const unsigned int m_frames_per_pixel;

  Autoscroll m_autoscroll;
  
};

#endif /*WIDGETRECOGNITIONAREA_HH_*/
