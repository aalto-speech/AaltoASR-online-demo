
#ifndef WIDGETSPECTROGRAM_HH_
#define WIDGETSPECTROGRAM_HH_

#include <fftw3.h>
#include "WidgetWave.hh"

class WidgetSpectrogram  :  public WidgetWave
{

public:

  WidgetSpectrogram(PG_Widget *parent, const PG_Rect &rect,
                    AudioInputController *audio_input, unsigned int pixels_per_second);
                    
  virtual ~WidgetSpectrogram();

  virtual void initialize();
  virtual void terminate();

protected:

  virtual void draw_new(unsigned long left_index, unsigned int oldview_size, unsigned int new_size);
  
private:

  fftw_plan m_coeffs;
  double *m_data_in;
  double *m_data_out;
  int m_window_width;
  
};

#endif /*WIDGETSPECTROGRAM_HH_*/
