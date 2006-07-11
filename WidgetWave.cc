
#include <math.h>
#include <SDL.h>
#include <algorithm>
//#include <minmax_element.hpp>
//#include <ororor>
#include "WidgetWave.hh"

WidgetWave::WidgetWave(PG_Widget *parent, const PG_Rect &rect,
                               AudioInputController *audio_input,
                               const unsigned int pixels_per_second)
  : PG_Widget(parent, rect, true), m_pixels_per_second(pixels_per_second),
    m_frames_per_pixel(SAMPLE_RATE / pixels_per_second)
{
  this->m_audio_input = audio_input;
  
  this->m_audio_buffer = NULL;
  this->m_surface_backbuffer = NULL;

  this->m_left_index = 0;
  this->m_last_left_index = 0;
  this->m_last_audio_data_size = 0;
}

WidgetWave::~WidgetWave()
{
  this->terminate();
}

void
WidgetWave::initialize()
{
  this->m_force_redraw = true;
  this->m_audio_buffer = new AUDIO_FORMAT[this->Width()*this->m_frames_per_pixel];
  memset(this->m_audio_buffer, '\0', this->Width()*this->m_frames_per_pixel*sizeof(AUDIO_FORMAT));
  
  // Create a back buffer so we don't need to redraw entirely.
  SDL_Surface *surface = this->GetWidgetSurface();
  this->m_width = surface->w;
  this->m_height = surface->h;
  this->m_surface_backbuffer = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                                    this->m_width,
                                                    this->m_height,
                                                    surface->format->BitsPerPixel,
                                                    surface->format->Rmask,
                                                    surface->format->Gmask,
                                                    surface->format->Bmask,
                                                    surface->format->Amask);
  this->m_foreground_color = SDL_MapRGB(surface->format, 255, 255, 0);
  this->m_background_color = SDL_MapRGB(surface->format, 10, 100, 20);
  SDL_FillRect(surface, NULL, this->m_background_color);
  // This filling is just for debbuging purposes. Blue should not be seen at anytime!
  SDL_FillRect(this->m_surface_backbuffer, NULL, SDL_MapRGB(surface->format,
                                                            0, 0, 255));
}

void
WidgetWave::terminate()
{
  delete this->m_audio_buffer;
  this->m_audio_buffer = NULL;

  if (this->m_surface_backbuffer) {
    SDL_FreeSurface(this->m_surface_backbuffer);
    this->m_surface_backbuffer = NULL;
  }
}


void
WidgetWave::reset()
{
  this->terminate();
  this->initialize();
}

unsigned int
WidgetWave::blit_old(unsigned long left_index)
{
  unsigned int oldview_from = 0, oldview_to = 0;
  unsigned int oldview_size = 0;
  int difference = left_index - this->m_last_left_index;

  if (difference > 0) {
    oldview_from = this->m_width < abs(difference) ? this->m_width : abs(difference);
  }
  else {
    oldview_to = this->m_width < abs(difference) ? this->m_width : abs(difference);
  }
  oldview_size = this->m_width - (oldview_from + oldview_to);

  // Do colliding view corrections if colliding area hasn't been entirely drawn.
  unsigned long audio_data_pixels = this->m_last_audio_data_size / this->m_frames_per_pixel;
  if (this->m_last_left_index + oldview_from + oldview_size > audio_data_pixels) {
    if (audio_data_pixels > this->m_last_left_index + oldview_from)
      oldview_size = audio_data_pixels - (this->m_last_left_index + oldview_from);
    else
      oldview_size = 0;
  }

  //blit old surface
  if (oldview_size && this->m_last_left_index != left_index) {
    SDL_Rect src_rect;
    SDL_Rect dest_rect;
    SDL_Surface *surface = this->GetWidgetSurface();
    src_rect.x = oldview_from;
    dest_rect.x = oldview_to;
    src_rect.y = dest_rect.y = 0;
    src_rect.w = dest_rect.w = oldview_size;
    src_rect.h = dest_rect.h = this->m_height;
    static char ch = 0;
    ch++;
    // filling is for debugging...
//    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 10, 100, 20));
    // this is the real blitting
    SDL_BlitSurface(this->m_surface_backbuffer, &src_rect, surface, &dest_rect);
  }
  
  return oldview_size;
}

unsigned long
WidgetWave::calculate_new(unsigned long left_index, unsigned long old_size)
{
  unsigned long newaudio_from = left_index * this->m_frames_per_pixel;
  long newaudio_size;
  if (left_index >= this->m_last_left_index)
    newaudio_from += old_size * this->m_frames_per_pixel;
  newaudio_size = this->m_frames_per_pixel * (this->m_width - old_size);
  unsigned long audio_data_size = this->m_audio_input->get_audio_data_size();
  if (newaudio_from + newaudio_size >= audio_data_size)
    newaudio_size = audio_data_size - newaudio_from;
  if (newaudio_size > 0) {
    memcpy(this->m_audio_buffer,
           this->m_audio_input->get_audio_data() + newaudio_from,
           newaudio_size * sizeof(AUDIO_FORMAT));
  }
  else {
    newaudio_size = 0;
  }
  this->m_last_audio_data_size = audio_data_size;
  return (unsigned long)newaudio_size;
}

void
WidgetWave::draw_new(unsigned long left_index, unsigned int oldview_size, unsigned int audio_size)
{
  SDL_Rect line_rect;
  SDL_Rect rect;
  SDL_Surface *surface = this->GetWidgetSurface();
  unsigned int audio_pixels = audio_size / this->m_frames_per_pixel;
  unsigned int x = (left_index >= this->m_last_left_index ? oldview_size : 0);

  rect.x = x;
  rect.w = left_index >= this->m_last_left_index ? (this->m_width - x) : audio_pixels;
  rect.y = 0;
  rect.h = surface->h;
  SDL_FillRect(surface, &rect, this->m_background_color);

  SDL_LockSurface(surface);

  for (unsigned int ind = 0;
       ind < audio_pixels;
       ind++)
  {
    AUDIO_FORMAT *first = this->m_audio_buffer + ind * this->m_frames_per_pixel;
    AUDIO_FORMAT max = *std::max_element(first, first + this->m_frames_per_pixel - 1);
    AUDIO_FORMAT min = *std::min_element(first, first + this->m_frames_per_pixel - 1);

    int y1 = (int)((double)(max / pow(256, sizeof(AUDIO_FORMAT)) * surface->h + 0.5f * (double)surface->h));
    int y2 = (int)((double)(min / pow(256, sizeof(AUDIO_FORMAT)) * surface->h + 0.5f * (double)surface->h));

    line_rect.x = x + ind;
    line_rect.w = 1;
    line_rect.y = y2;
    line_rect.h = y1 - y2 + 1;

    SDL_FillRect(surface, &line_rect, this->m_foreground_color);
  }
//*/
  SDL_UnlockSurface(surface);
}

void
WidgetWave::update()
{
  // This is for thread-safety.
  unsigned long left_index = this->m_left_index;
  unsigned int oldview_size = 0;

  if (!this->m_force_redraw) {
    // Calculates and blits old view content.
    oldview_size = this->blit_old(left_index);
  }
  
  // Get the new audio data to draw.
  unsigned long newaudio_size = this->calculate_new(left_index, oldview_size);

  // Draw the new parts of the view.
  this->draw_new(left_index, oldview_size, newaudio_size);

  // Update variables and screen.
  this->m_last_left_index = left_index;
  this->m_force_redraw = false;
  SDL_BlitSurface(this->GetWidgetSurface(), NULL, this->m_surface_backbuffer, NULL);
  this->Update(true);
}
