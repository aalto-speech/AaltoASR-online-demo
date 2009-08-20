#include <algorithm>
#include <math.h>
#include "WidgetWave.hh"

WidgetWave::WidgetWave(PG_Widget *parent,
                       const PG_Rect &rect,
                       AudioInputController *audio_input,
                       const unsigned int pixels_per_second)
  : WidgetAudioView(parent, rect, audio_input, pixels_per_second)
{

}

void
WidgetWave::initialize()
{
  WidgetAudioView::initialize();
  
  SDL_Surface *surface = this->GetWidgetSurface();
  this->m_foreground_color = SDL_MapRGB(surface->format, 255, 255, 0);
  this->set_background_color(SDL_MapRGB(surface->format, 10, 100, 20));
}

void
WidgetWave::draw_screen_vector(SDL_Surface *surface, unsigned int x)
{
  const AUDIO_FORMAT *audio_data = this->m_audio_input->get_audio_data();
  SDL_Rect line_rect;

  unsigned int index = (int)((x + this->m_scroll_pos) * this->m_samples_per_pixel);
  const AUDIO_FORMAT *first = audio_data + index;
  AUDIO_FORMAT max = *std::max_element(first, first + (int)this->m_samples_per_pixel);
  AUDIO_FORMAT min = *std::min_element(first, first + (int)this->m_samples_per_pixel);

  static double scale = surface->h / pow(256, sizeof(AUDIO_FORMAT));
  int y1 = (int)(max * scale + 0.5 * surface->h);
  int y2 = (int)(min * scale + 0.5 * surface->h);

  line_rect.x = x;
  line_rect.w = 1;
  line_rect.y = y2;
  line_rect.h = y1 - y2 + 1;

  SDL_FillRect(surface, &line_rect, this->m_foreground_color);
}
