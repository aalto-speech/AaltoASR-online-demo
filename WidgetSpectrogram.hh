
#ifndef WIDGETSPECTROGRAM_HH_
#define WIDGETSPECTROGRAM_HH_

#include <fftw3.h>
//#include "WidgetWave.hh"
#include "WidgetAudioView.hh"

class WidgetSpectrogram  :  public WidgetAudioView//WidgetWave
{

public:

  WidgetSpectrogram(PG_Widget *parent,
                    const PG_Rect &rect,
                    AudioInputController *audio_input,
                    unsigned int pixels_per_second);
                    
  virtual ~WidgetSpectrogram();

  virtual void initialize();
  virtual void terminate();

protected:

//  virtual Uint32 get_background_color(SDL_PixelFormat *format) const;

  virtual void fix_oldview_size(unsigned int oldview_from,
                                unsigned int &oldview_size);

  void write_data_in(unsigned int audio_pixel);//, unsigned int audio_size);
  void calculate_output_values();
  virtual void create_y_axis();
  void draw_screen_vector(SDL_Surface *surface, unsigned int x);
  void do_drawing(SDL_Surface *surface, unsigned int x);
  static double interpolate(double *array, double index);

  Uint32 get_color_by_value(double value);

  static void draw_pixel(SDL_Surface *surface,
                         unsigned int x,
                         unsigned int y,
                         Uint32 color);
  
  
private:

  fftw_plan m_coeffs;
  double *m_data_in;
  double *m_data_out;
  unsigned int m_window_width;
  double *m_y_axis;
//  double *m_screen_vector_values;
  
};

#endif /*WIDGETSPECTROGRAM_HH_*/
