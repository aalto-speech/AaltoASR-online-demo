#include <math.h>
#include <algorithm>
#include "WidgetSpectrogram.hh"

WidgetSpectrogram::WidgetSpectrogram(PG_Widget *parent,
                                     const PG_Rect &rect,
                                     AudioInputController *audio_input,
                                     unsigned int pixels_per_second,
                                     double magnitude_exponent,
                                     double magnitude_suppressor)
  : WidgetAudioView(parent, rect, audio_input, pixels_per_second),
    m_magnitude_exponent(magnitude_exponent),
    m_magnitude_suppressor(magnitude_suppressor)
{
  this->m_window_width = 256;//1000;//256;//2 * this->my_height;//
  this->m_data_in = new double[this->m_window_width];
  this->m_data_out = new double[this->m_window_width+1];
  this->m_data_out[this->m_window_width] = 0;
  
  // Linear y-axis by default.
  this->m_y_axis = NULL;
  this->create_y_axis(1.0, 1.0);

  // Initialize fast fourier transformation.
  this->m_coeffs = fftw_plan_r2r_1d(this->m_window_width,
                                    this->m_data_in,
                                    this->m_data_out,
                                    FFTW_R2HC,
                                    FFTW_ESTIMATE);
}

WidgetSpectrogram::~WidgetSpectrogram()
{
  fftw_destroy_plan(this->m_coeffs);
  delete [] this->m_data_in;
  delete [] this->m_data_out;

  delete [] this->m_y_axis;
  this->m_y_axis = NULL;
}

void
WidgetSpectrogram::set_magnitude_exponent(double exponent)
{
  this->m_magnitude_exponent = exponent;
  this->m_force_redraw = true;
}

void
WidgetSpectrogram::set_magnitude_suppressor(double suppressor)
{
  this->m_magnitude_suppressor = suppressor;
  this->m_force_redraw = true;
}

void
WidgetSpectrogram::create_y_axis(double linear_height, double linear_data)
{
  // Check that parameters are valid.
  assert(this->my_height > 1);
  assert(linear_height >= 0.0 && linear_height <= 1.0);
  assert(linear_data > 0.0 && linear_data <= 1.0);

  delete [] this->m_y_axis;
  this->m_y_axis = new double[this->my_height];

  for (unsigned int ind = 0; ind < this->my_height; ind++) {
    // Linear y-axis in range [0,1]. (Also base for the other axises.)
    this->m_y_axis[ind] = (double)ind / (this->my_height - 1);
    
    //  Part linear and part logarithmic y-axis.
    // (These code lines may be commented out if linear y-axis is wanted.)
    //*
    if (this->m_y_axis[ind] < linear_height || linear_height >= 1.0) {
      this->m_y_axis[ind] = this->m_y_axis[ind] / linear_height * linear_data;
    }
    else {
      const double max_value = 1.0;
      const double min_value = linear_data;
      double exponent = (this->m_y_axis[ind] - linear_height) / (1.0 - linear_height);
      this->m_y_axis[ind] = min_value * pow(max_value / min_value, exponent);
    }
    //*/

    //  Logarithmic y-axis. Range (0,1]. (These code lines may be commented out
    // if linear y-axis is wanted.)
    /*
    const double max_value = 1.0;
    const double min_value = 0.01;
    this->m_y_axis[ind] = min_value * pow(max_value / min_value, this->m_y_axis[ind]);
    //*/
    
    // Now scale the axis so that it will point to fftw data_out indexes.
    this->m_y_axis[ind] = this->m_y_axis[ind] * (this->m_window_width / 2 - 1);
  }

  // When axis is modified, we need to redraw the entire view.
  this->m_force_redraw = true;
}

void
WidgetSpectrogram::fix_oldview_size(unsigned int oldview_from,
                                    unsigned int &oldview_size)
{
  unsigned long first_pixel = this->m_last_scroll_pos + oldview_from;

  // Calculate pixels that had enough information when they were drawn.
  // Pixel might be drawn with less than m_window_width information if there
  // was not enough audio data.
  unsigned long correct_audio_pixels;
  if (this->m_last_audio_data_size > this->m_window_width)
    correct_audio_pixels = (long)((this->m_last_audio_data_size - this->m_window_width) / this->m_samples_per_pixel);
  else
    correct_audio_pixels = 0;

  // Reduce the oldview_size if pixels inside the oldview didn't have
  // enough (or at all) audio information when they were drawn.  
  if (first_pixel + oldview_size > correct_audio_pixels) {
    if (correct_audio_pixels > first_pixel)
      oldview_size = correct_audio_pixels - first_pixel;
    else
      oldview_size = 0;
  }
}

void
WidgetSpectrogram::draw_screen_vector(SDL_Surface *surface, unsigned int x)
{
  this->write_data_in(x + this->m_scroll_pos);
  fftw_execute(this->m_coeffs);
  this->calculate_output_values();
  this->do_drawing(surface, x);
}

void
WidgetSpectrogram::write_data_in(unsigned int audio_pixel)
{
  unsigned int audio_window_size = this->m_window_width;
  unsigned long audio_size = this->m_audio_input->get_audio_data_size();
  const AUDIO_FORMAT *audio_data = this->m_audio_input->get_audio_data();
  
  // Check if not enough audio data for the fftw window.
  if (audio_pixel * this->m_samples_per_pixel + this->m_window_width > audio_size) {
    memset(this->m_data_in, 0, sizeof(double) * this->m_window_width);
    audio_window_size = (int)(audio_size - audio_pixel * this->m_samples_per_pixel);
  }
  
  // Write data from audio buffer to data input buffer.
  for (unsigned jnd = 0; jnd < audio_window_size; jnd++) {
    unsigned int index = (int)(jnd + audio_pixel * this->m_samples_per_pixel);
    this->m_data_in[jnd] = (double)audio_data[index];
  }
}

void
WidgetSpectrogram::calculate_output_values()
{
  unsigned int range = this->m_window_width / 2;
  
  for (unsigned int ind = 0; ind < range; ind++) {
    this->m_data_out[ind] = this->m_data_out[ind] * this->m_data_out[ind];
    // Avoid: value = x[i]^2 + x[i]^2 . Should be x[i]^2 + x[j]^2 (i!=j).
    if (ind != this->m_window_width - ind) {
      unsigned int index = this->m_window_width - ind;
      this->m_data_out[ind] += this->m_data_out[index] * this->m_data_out[index];
    }
    
    // Root.
    // ADJUST: The exponent is the FIRST parameter for the spectrogram. You can
    // adjust it to change the appearance of the spectrogram.
    this->m_data_out[ind] = pow(this->m_data_out[ind],
                                this->m_magnitude_exponent);
  }

  // Do normalization (values into range 0.0-1.0), and use some non-linear
  // function (e.g. ^0.1) to make spectrogram clearer.
  for (unsigned int ind = 0; ind < range; ind++) {
    // ADJUST: The base number is the SECOND parameter for the spectrogram. You
    // can adjust it to change the appearance of the spectrogram.
    this->m_data_out[ind] = 1 - pow(this->m_magnitude_suppressor,
                                    this->m_data_out[ind]);
  }
}

Uint32
WidgetSpectrogram::get_color_by_value(double value)
{
  char r = 0;
  char g = 0;
  char b = 0;

  // Color mapping.  
  if (value <= 0.25) {
    b = (char)(value / 0.25 * 128);
  }
  else if (value > 0.25 && value <= 0.50) {
    r = (char)((value - 0.25) / (0.50-0.25) * 255);
    b = (char)((0.50 - value) / (0.50-0.25) * 128);
  }
  else if (value > 0.50 && value <= 1.0) {
    r = 255;
    g = (char)((value - 0.50) / (1.0 - 0.50) * 255);
  }
  
  return SDL_MapRGB(this->GetWidgetSurface()->format, r, g, b);
}

void
WidgetSpectrogram::draw_pixel(SDL_Surface *surface,
                              unsigned int x,
                              unsigned int y,
                              Uint32 color)
{
  // Draw the pixel.
  unsigned int bpp = surface->format->BytesPerPixel;
  for (unsigned int knd = 0; knd < bpp; knd++) {
    unsigned int index = bpp * (x + y * surface->w) + knd;
    ((char*)surface->pixels)[index] = ((char*)&color)[knd];
  }
}

void
WidgetSpectrogram::do_drawing(SDL_Surface *surface, unsigned int x)
{
  assert(this->my_height > 1);
  double value;
  
  for (unsigned int ynd = 0; ynd < this->my_height; ynd++) {
    // Interpolate the value.
    value = this->interpolate(this->m_data_out, this->m_y_axis[ynd]);

    // Do color mapping.
    Uint32 color = this->get_color_by_value(value);
    
    // Draw the pixel.
    this->draw_pixel(surface, x, (this->my_height - 1) - ynd, color);
  }
}

double
WidgetSpectrogram::interpolate(double *array, double index)
{
  int low = (int)index;
  int high = (int)ceil(index);
  if (low == high) {
    return array[low];
  }
  else {
    return (high - index) * array[low] + (index - low) * array[high];
  }
}
