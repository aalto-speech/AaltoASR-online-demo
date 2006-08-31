
#ifndef WIDGETSPECTROGRAM_HH_
#define WIDGETSPECTROGRAM_HH_

#include <fftw3.h>
#include "WidgetAudioView.hh"

class WidgetSpectrogram  :  public WidgetAudioView
{

public:

  /** 
   * 
   * \param magnitude_exponent Makes the relative differences between magnitudes
   *                           smaller. Should be in range (0,1]. The smaller
   *                           the value, the smaller the differences get.
   *                           A value between 0.2-0.5 might be good. Used as
   *                           "mag = mag ^ mag_exp".
   * \param magnitude_suppressor Transforms the magnitude into range [0,1) by
   *                             doing a non-linear transformation:
   *                             "mag = 1 - suppressor ^ mag".
   *                             Values like 0.95-0.99999 might be good. Note
   *                             that the previous magnitude_exponent parameter
   *                             also affects the result of this transformation!
   *                             So when adjusting magnitude_exponent, you may
   *                             want to adjust this parameter also. The closer
   *                             the value is to 1.0 the more the magnitudes
   *                             are suppressed. */
  WidgetSpectrogram(PG_Widget *parent,
                    const PG_Rect &rect,
                    AudioInputController *audio_input,
                    unsigned int pixels_per_second,
                    double magnitude_exponent,
                    double magnitude_suppressor);
                    
  virtual ~WidgetSpectrogram();

  /** Creates a mapping for the y axis.
   * \param linear_height How big part is linear in the view. A value in
   *                      range [0.0, 1.0]. Normally a value 0.5-0.8 is
   *                      what you might want.
   *                      0 = no linearity at all, 1 = completely linear.
   * \param linear_data The size of the data to scale into the linear part of
   *                    the view. A value in range (0.0, 1.0]. Normally a
   *                    value 0.1-0.4 might be what you want. Logarithmic
   *                    view shows data starting from this value. It must be
   *                    more than zero. */
  void create_y_axis(double linear_height, double linear_data);
  
  inline double get_magnitude_exponent() const;
  inline double get_magnitude_suppressor() const;
  void set_magnitude_exponent(double exponent);
  void set_magnitude_suppressor(double suppressor);

protected:

  /** Fixes the old blittable area by reducing area that wasn't actually
   * drawn yet (or was drawn incorrectly, e.g. wasn't enough data). The end of
   * the spectrogram may be drawn incorrectly if there's not enough audio data
   * for the fftw window.
   * \param oldview_from X coordinate of the blitting area in the previous view.
   * \param oldview_size Size of the area to blit is fixed. */
  virtual void fix_oldview_size(unsigned int oldview_from,
                                unsigned int &oldview_size);

  /** Writes input for the fftw.
   * \param audio_pixel Starting pixel of the audio data to fftw window. */
  void write_data_in(unsigned int audio_pixel);

  /** Do some transformations to the fftw output. */
  void calculate_output_values();

  /** Draws a pixel vector. Writes the fftw input, executes the fftw, calculates
   * the output values and does the actual drawing. */
  virtual void draw_screen_vector(SDL_Surface *surface, unsigned int x);

  /** Does the actual drawing according to the fftw output. They should be
   * transformed before calling this function. */
  void do_drawing(SDL_Surface *surface, unsigned int x);
  
  /** \return The color value for the given magnitude value [0,1]. */
  Uint32 get_color_by_value(double value);

  /** An array index may be a double, so it interpolates the value from the
   * two integer indexes. */
  static double interpolate(double *array, double index);

  /** Draws a pixel. */
  static void draw_pixel(SDL_Surface *surface,
                         unsigned int x,
                         unsigned int y,
                         Uint32 color);
  
  
private:

  // Variables for fftw.
  fftw_plan m_coeffs;
  double *m_data_in;
  double *m_data_out;
  unsigned int m_window_width;
  
  /** Mapping of output indexes to screen y coordinates of the view. */
  double *m_y_axis;

  /** First transformation: mag = mag ^ exponent. Exponent is in range (0,1].
   * Smaller value reduces the relative differences between magnitudes, compare
   * to "contrast".*/
  double m_magnitude_exponent;
  /** Second transformation: mag = 1 - suppressor ^ mag. Non linearly transforms
   * the magnitude into range [0,1). Suppressor is in range (0,1).
   * The closer the value is to 1, the darker the spectrogram is. */
  double m_magnitude_suppressor;
  
};

double
WidgetSpectrogram::get_magnitude_exponent() const
{
  return this->m_magnitude_exponent;
}

double
WidgetSpectrogram::get_magnitude_suppressor() const
{
  return this->m_magnitude_suppressor;
}

#endif /*WIDGETSPECTROGRAM_HH_*/
