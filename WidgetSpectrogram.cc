
#include <algorithm>
#include "WidgetSpectrogram.hh"

WidgetSpectrogram::WidgetSpectrogram(PG_Widget *parent,
                                     const PG_Rect &rect,
                                     AudioInputController *audio_input,
                                     unsigned int pixels_per_second)
  : WidgetWave(parent, rect, audio_input, pixels_per_second)
{
//  this->m_window_width = SAMPLE_RATE / pixels_per_second;
  this->m_window_width = 2 * this->my_height;
  this->m_data_in = new double[this->m_window_width];
  this->m_data_out = new double[this->m_window_width+1];
  this->m_data_out[this->m_window_width] = 0;

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
}

void
WidgetSpectrogram::initialize()
{
  WidgetWave::initialize();
  SDL_FillRect(this->GetWidgetSurface(), NULL, 0);
}

void
WidgetSpectrogram::terminate()
{
  WidgetWave::terminate();
}

void
WidgetSpectrogram::draw_new(unsigned long left_index,
                            unsigned int oldview_size,
                            unsigned int audio_size)
{
  SDL_Surface *surface = this->GetWidgetSurface();
  unsigned int audio_pixels = audio_size / this->m_frames_per_pixel;
  unsigned int x = (left_index >= this->m_last_left_index ? oldview_size : 0);

  SDL_LockSurface(surface);

  for (unsigned int ind = 0; ind < audio_pixels; ind++)
  {
    //*
    unsigned int audio_window_size = this->m_window_width;
    if (ind * this->m_frames_per_pixel + this->m_window_width > audio_size) {
      memset(this->m_data_in, 0, sizeof(double) * this->m_window_width);
      audio_window_size = audio_size - ind * this->m_frames_per_pixel;
    }
    for (unsigned jnd = 0; jnd < audio_window_size; jnd++) {
      this->m_data_in[jnd] = this->m_audio_buffer[jnd+ind*this->m_frames_per_pixel];
    }
    //*/
    /*
    for (unsigned jnd = 0; jnd < this->m_window_width; jnd++) {
      this->m_data_in[jnd] = this->m_audio_buffer[jnd+ind*this->m_frames_per_pixel];
    }
    //*/

    fftw_execute(this->m_coeffs);

    // Calculate values.
    double *values = new double[this->my_height];
    for (unsigned int jnd = 0; jnd < this->my_height; jnd++) {
      unsigned int index = this->m_window_width / 2 * jnd / this->my_height;
      values[jnd] = this->m_data_out[index] * this->m_data_out[index];
      if (index != this->m_window_width - index) {
        index = this->m_window_width - index;
        values[jnd] += this->m_data_out[index] * this->m_data_out[index];
      }
//      values[jnd] = jnd;
    }

    double min = *std::min_element(values, values + this->my_height);
    double max = *std::max_element(values, values + this->my_height);
    
    for (unsigned int jnd = 0; jnd < this->my_height; jnd++) {
      double value = (values[this->my_height-jnd-1] - min) / max;
      /*
      value *= (exp(1) - 1);
      value += 1;
      value = log(value);
      value = pow(value, 0.4);
      //*/
      value = pow(value, 0.3);
      Uint32 color = SDL_MapRGB(surface->format, (char)(value * 255), (char)(value * 255), 0);
      unsigned int bpp = surface->format->BytesPerPixel;
      for (unsigned int knd = 0; knd < bpp; knd++) {
        unsigned int index = knd + (x + ind + this->my_width * jnd) * bpp;
        ((char*)surface->pixels)[index] = ((char*)&color)[knd];
      }
    }
  }

  SDL_UnlockSurface(surface);
}
