
#include "WidgetSpectrogram.hh"

WidgetSpectrogram::WidgetSpectrogram(PG_Widget *parent,
                                     const PG_Rect &rect,
                                     AudioInputController *audio_input,
                                     unsigned int pixels_per_second)
  : WidgetWave(parent, rect, audio_input, pixels_per_second)
{
  this->m_window_width = 2 * SAMPLE_RATE / 125;
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
  fprintf(stderr, "WS update before\n");
  SDL_Rect line_rect;
  SDL_Rect rect;
  SDL_Surface *surface = this->GetWidgetSurface();
  unsigned int audio_pixels = audio_size / this->m_frames_per_pixel;
  unsigned int x = (left_index >= this->m_last_left_index ? oldview_size : 0);

  rect.x = x;
  rect.w = left_index >= this->m_last_left_index ? (this->m_width - x) : audio_pixels;
  rect.y = 0;
  rect.h = surface->h;
  
  SDL_LockSurface(surface);

  for (unsigned int ind = 0;
       ind < audio_pixels;
       ind++)
  {
//    this->m_audio_input->get_audio_data()
/*
    AUDIO_FORMAT *first = this->m_audio_buffer + ind * this->m_frames_per_pixel;
    AUDIO_FORMAT max = *std::max_element(first, first + this->m_frames_per_pixel - 1);
    AUDIO_FORMAT min = *std::min_element(first, first + this->m_frames_per_pixel - 1);

    int y1 = (int)((double)(max / pow(256, sizeof(AUDIO_FORMAT)) * surface->h + 0.5f * (double)surface->h));
    int y2 = (int)((double)(min / pow(256, sizeof(AUDIO_FORMAT)) * surface->h + 0.5f * (double)surface->h));

    line_rect.x = x + ind;
    line_rect.w = 1;
    line_rect.y = y2;
    line_rect.h = y1 - y2 + 1;

    SDL_FillRect(surface, &line_rect, color);
    //*/
  }
//*/
  SDL_UnlockSurface(surface);
  fprintf(stderr, "WS update after\n");
}
