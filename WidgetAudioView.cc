
#include <math.h>
#include <SDL.h>
#include <algorithm>
#include "WidgetWave.hh"
#include "AudioStream.hh"

WidgetAudioView::WidgetAudioView(PG_Widget *parent,
                                 const PG_Rect &rect,
                                 AudioInputController *audio_input,
                                 const unsigned int pixels_per_second)
  : PG_Widget(parent, rect, true),
    m_pixels_per_second(pixels_per_second),
    m_samples_per_pixel((double)audio::audio_sample_rate / (double)pixels_per_second)
{
  this->m_audio_input = audio_input;
  
  this->m_surface_backbuffer = NULL;

  this->m_scroll_pos = 0;
  this->m_last_scroll_pos = 0;
  this->m_last_audio_data_size = 0;
  
  this->m_background_color = 0;

}

WidgetAudioView::~WidgetAudioView()
{
  this->terminate();
}

void
WidgetAudioView::initialize()
{
  this->m_force_redraw = true;
  
  // Create a back buffer so we don't need to redraw entirely.
  SDL_Surface *surface = this->GetWidgetSurface();
  this->m_surface_backbuffer = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                                    this->my_width,
                                                    this->my_height,
                                                    surface->format->BitsPerPixel,
                                                    surface->format->Rmask,
                                                    surface->format->Gmask,
                                                    surface->format->Bmask,
                                                    surface->format->Amask);

  SDL_FillRect(surface, NULL, this->m_background_color);

}

void
WidgetAudioView::terminate()
{
  if (this->m_surface_backbuffer) {
    SDL_FreeSurface(this->m_surface_backbuffer);
    this->m_surface_backbuffer = NULL;
  }
}


void
WidgetAudioView::reset()
{
  this->terminate();
  this->initialize();
}

void
WidgetAudioView::fix_oldview_size(unsigned int oldview_from,
                                  unsigned int &oldview_size)
{
  unsigned long audio_data_pixels = 
    (unsigned long)(this->m_last_audio_data_size / this->m_samples_per_pixel);
    
  if (this->m_last_scroll_pos + oldview_from + oldview_size > audio_data_pixels) {
    if (audio_data_pixels > this->m_last_scroll_pos + oldview_from)
      oldview_size = audio_data_pixels - (this->m_last_scroll_pos + oldview_from);
    else
      oldview_size = 0;
  }
}

void
WidgetAudioView::calculate_old(unsigned int &oldview_from,
                               unsigned int &oldview_to,
                               unsigned int &oldview_size)
{
  int difference = this->m_scroll_pos - this->m_last_scroll_pos;

  if (difference > 0) {
    oldview_from = this->my_width < abs(difference) ? this->my_width : abs(difference);
    oldview_to = 0;
  }
  else {
    oldview_from = 0;
    oldview_to = this->my_width < abs(difference) ? this->my_width : abs(difference);
  }
  
  oldview_size = this->my_width - (oldview_from + oldview_to);

  // Do old view correction for cases the area hasn't been entirely drawn.
  this->fix_oldview_size(oldview_from, oldview_size);
}

unsigned int
WidgetAudioView::blit_old()
{
  unsigned int oldview_from, oldview_to, oldview_size;
  
  this->calculate_old(oldview_from, oldview_to, oldview_size);

  // Blit old view so we don't need to redraw it.
  if (oldview_size) {
    SDL_Rect src_rect;
    SDL_Rect dest_rect;
    SDL_Surface *surface = this->GetWidgetSurface();
    src_rect.x = oldview_from;
    dest_rect.x = oldview_to;
    src_rect.y = dest_rect.y = 0;
    src_rect.w = dest_rect.w = oldview_size;
    src_rect.h = dest_rect.h = this->my_height;

    SDL_BlitSurface(this->m_surface_backbuffer, &src_rect, surface, &dest_rect);
    fprintf(stderr, "blitting\n");
  }
  
  return oldview_size;
}

void
WidgetAudioView::calculate_new(unsigned long old_size,
                               unsigned long &newaudio_from,
                               unsigned long &newaudio_size)
{
  newaudio_from = (long)(this->m_scroll_pos * this->m_samples_per_pixel);
  unsigned long audio_data_size = this->m_audio_input->get_audio_data_size();

  // Check whether the new visible audio comes from left or right.  
  if (this->m_scroll_pos >= this->m_last_scroll_pos)
    newaudio_from += (long)(old_size * this->m_samples_per_pixel);

  // Calculate the size of the new view.
  newaudio_size = (long)(this->m_samples_per_pixel * (this->my_width - old_size));
  if (newaudio_from + newaudio_size >= audio_data_size) {
    if (audio_data_size > newaudio_from)
      newaudio_size = audio_data_size - newaudio_from;
    else
      newaudio_size = 0;
  }
  
  this->m_last_audio_data_size = audio_data_size;
}

void
WidgetAudioView::draw_new(unsigned int x,
                          unsigned long newaudio_from,
                          unsigned long newaudio_size)
{
  SDL_Rect rect;
  SDL_Surface *surface = this->GetWidgetSurface();
  unsigned int audio_pixels = (int)(newaudio_size / this->m_samples_per_pixel);

  rect.x = x;
  rect.w = this->m_scroll_pos >= this->m_last_scroll_pos ? (this->my_width - x) : audio_pixels;
  rect.y = 0;
  rect.h = surface->h;
  SDL_FillRect(surface, &rect, this->m_background_color);

  SDL_LockSurface(surface);
  for (unsigned int ind = 0; ind < audio_pixels; ind++)
    this->draw_screen_vector(surface, x + ind);
  SDL_UnlockSurface(surface);
}

void
WidgetAudioView::update()
{
  unsigned int oldview_size = 0;
  unsigned long newaudio_from, newaudio_size;

  if (!this->m_force_redraw) {
    // Calculates and blits old view content.
    oldview_size = this->blit_old();
  }
  
  // Calculate the new visible audio.
  this->calculate_new(oldview_size, newaudio_from, newaudio_size);

  // Draw the new parts of the view.
  if (newaudio_size) {
    unsigned int x =
      (this->m_scroll_pos >= this->m_last_scroll_pos ? oldview_size : 0);
      
    this->draw_new(x, newaudio_from, newaudio_size);
  }

  // Update variables and backbuffer.
  this->m_last_scroll_pos = this->m_scroll_pos;
  this->m_force_redraw = false;
  SDL_BlitSurface(this->GetWidgetSurface(), NULL, this->m_surface_backbuffer, NULL);
}

bool
WidgetAudioView::eventMouseButtonUp(const SDL_MouseButtonEvent *button)
{
  if (button->button == SDL_BUTTON_LEFT) {
    if (this->m_audio_input->is_playbacking()) {
      this->m_audio_input->stop_playback();
    }
    else {
      long cursor_px = this->m_scroll_pos + (button->x - this->my_xpos);
      if (cursor_px >= 0) {
        unsigned long cursor_audio = (long)(this->m_samples_per_pixel * cursor_px);
        this->m_audio_input->start_playback(cursor_audio);
      }
    }
    return true;
  }
  else if (button->button == SDL_BUTTON_RIGHT) {
    this->m_audio_input->stop_playback();
    return true;
  }
  return false;
}
