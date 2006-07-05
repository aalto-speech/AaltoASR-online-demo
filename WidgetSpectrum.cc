
#include <math.h>
#include <SDL.h>
#include <algorithm>
//#include <minmax_element.hpp>
//#include <ororor>
#include "WidgetSpectrum.hh"

WidgetSpectrum::WidgetSpectrum(PG_Widget *parent, const PG_Rect &rect,
                               AudioInputController *audio_input,
                               const unsigned int pixels_per_second)
  : PG_Widget(parent, rect, true), m_pixels_per_second(pixels_per_second),
    m_frames_per_pixel(SAMPLE_RATE / pixels_per_second)
{
//  pthread_mutex_init(&this->m_leftindex_lock, NULL);
  this->m_audio_input = audio_input;
  this->initialize();
}

WidgetSpectrum::~WidgetSpectrum()
{
//  pthread_mutex_destroy(&this->m_leftindex_lock);
//  delete this->m_audio_buffer;
//  SDL_FreeSurface(this->m_surface_backbuffer);
  this->terminate();
}

void
WidgetSpectrum::initialize()
{
  this->m_left_index = 0;
  this->m_last_left_index = 0;
  this->m_last_audio_data_size = 0;
//  this->m_force_redraw = true;                                                

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
  SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 10, 100, 20));
  // This filling is just for debbuging purposes. Blue should not be seen at anytime!
  SDL_FillRect(this->m_surface_backbuffer, NULL, SDL_MapRGB(surface->format,
                                                            0, 0, 255));
}

void
WidgetSpectrum::terminate()
{
  delete this->m_audio_buffer;
  this->m_audio_buffer = NULL;
  SDL_FreeSurface(this->m_surface_backbuffer);
  this->m_surface_backbuffer = NULL;
}


void
WidgetSpectrum::reset()
{
  this->terminate();
  this->initialize();
}

void
WidgetSpectrum::update()
{
/*
  // Update left border.
  unsigned long read_cursor_px = this->m_audio_input->get_read_cursor() / this->m_frames_per_pixel;
  if (read_cursor_px > this->m_width)
    this->m_left_index = read_cursor_px - this->m_width;
  else
    this->m_left_index = 0;
//*/

  // This is for thread-safety.
  unsigned long left_index = this->m_left_index;

  // Calculate the colliding view area.
  unsigned int oldview_from = 0, oldview_to = 0;
  unsigned int oldview_size;
  int difference = left_index - this->m_last_left_index;
//  if (difference == 0)
//    return;
  if (difference > 0) {
    oldview_from = this->m_width < abs(difference) ? this->m_width : abs(difference);
  }
  else {
    oldview_to = this->m_width < abs(difference) ? this->m_width : abs(difference);
  }
  oldview_size = this->m_width - (oldview_from + oldview_to);
/*  
  if (this->m_force_redraw) {
    oldview_size = 0;
    this->m_force_redraw = false;
  }
//*/

  // Do colliding view corrections if colliding area hasn't been entirely drawn.
  unsigned long audio_data_pixels = this->m_last_audio_data_size / this->m_frames_per_pixel;
  if (this->m_last_left_index + oldview_from + oldview_size > audio_data_pixels) {
    if (audio_data_pixels > this->m_last_left_index + oldview_from)
      oldview_size = audio_data_pixels - (this->m_last_left_index + oldview_from);
    else
      oldview_size = 0;
  }

  // Get the new audio data to draw.
  unsigned long newaudio_from = left_index * this->m_frames_per_pixel;
  long newaudio_size;
  if (left_index >= this->m_last_left_index)
    newaudio_from += oldview_size * this->m_frames_per_pixel;
  newaudio_size = this->m_frames_per_pixel * (this->m_width - oldview_size);
//  this->m_audio_input->lock_audio_writing();
  unsigned long audio_data_size = this->m_audio_input->get_audio_data_size();
  if (newaudio_from + newaudio_size >= audio_data_size)
    newaudio_size = audio_data_size - newaudio_from;
  if (newaudio_size > 0) {
    memcpy(this->m_audio_buffer,
           this->m_audio_input->get_audio_data() + newaudio_from,
           newaudio_size * sizeof(AUDIO_FORMAT));
//  fprintf(stderr, "Here.\n"); fflush(stderr);
  }
  else {
    newaudio_size = 0;
  }
//  this->m_audio_input->unlock_audio_writing();
  
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
  
  //draw new surface
  SDL_Rect rect;
  SDL_Surface *surface = this->GetWidgetSurface();
  Uint32 color = SDL_MapRGB(surface->format, 255, 255, 0);
  unsigned int bfrom = 0;
  unsigned int bsize = newaudio_size / this->m_frames_per_pixel;
  if (left_index >= this->m_last_left_index)
    bfrom = oldview_size;
  SDL_LockSurface(surface);
  char *data = (char*)surface->pixels;
  int bpp = surface->format->BytesPerPixel;
  rect.x = bfrom;
  rect.w = left_index >= this->m_last_left_index ? (this->m_width - bfrom) : bsize;
  rect.y = 0;
  rect.h = surface->h;
  SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 10, 100, 20));
//  if (bsize == 0)
//    fprintf(stderr, "HEI!\n");
  //*
  for (unsigned int ind = 0;
       ind < bsize;
       ind++)
//       ind < surface->w && ind * frames_per_pixel + left_border < audio_size;
//       ind++)
  {
    AUDIO_FORMAT *first = this->m_audio_buffer + ind * this->m_frames_per_pixel;
    AUDIO_FORMAT max = *std::max_element(first, first + this->m_frames_per_pixel - 1);
    AUDIO_FORMAT min = *std::min_element(first, first + this->m_frames_per_pixel - 1);
//                                   this->m_audio_buffer this->m_frames_per_pixel) 
    int y1 = (int)((double)(max / pow(256, sizeof(AUDIO_FORMAT)) * surface->h + 0.5f * (double)surface->h));
    int y2 = (int)((double)(min / pow(256, sizeof(AUDIO_FORMAT)) * surface->h + 0.5f * (double)surface->h));
//    y1 = (int)((double)(max(audio_data + ind*frames_per_pixel + left_border, frames_per_pixel) / pow(256, sizeof(AUDIO_FORMAT)) * surface->h + 0.5f * (double)surface->h));
//    y2 = (int)((double)(min(audio_data + ind*frames_per_pixel + left_border, frames_per_pixel) / pow(256, sizeof(AUDIO_FORMAT)) * surface->h + 0.5f * (double)surface->h));
    if (y1 < 0 ) y1 = 0;
    if (y1 >= this->m_height) y1 = this->m_height - 1;
    if (y2 < 0 ) y2 = 0;
    if (y2 >= this->m_height) y2 = this->m_height - 1;
    rect.x = bfrom + ind;
    rect.w = 1;
    rect.y = y2;
    rect.h = y1 - y2 + 1;
    /*
    for (unsigned int jnd = 0; jnd < bpp; jnd++) {
      data[bpp*(rect.x+surface->w*y1)+jnd] = ((char*)&color)[jnd];
      data[bpp*(rect.x+surface->w*y2)+jnd] = ((char*)&color)[jnd];
    }
    //*/
    SDL_FillRect(surface, &rect, color);
//      this->DrawVLine(ind, y2, y1 - y2 + 1, PG_Color(255, 255, 0));
  }
  //*/
  SDL_UnlockSurface(surface);

  //update
  this->m_last_audio_data_size = audio_data_size;
  this->m_last_left_index = left_index;
  SDL_BlitSurface(surface, NULL, this->m_surface_backbuffer, NULL);
  this->Update(true);

  //*/

/*
  
  SDL_Surface *surface = this->GetWidgetSurface();
  SDL_Rect rect;
  unsigned int bpp;
  int y1, y2;
  static unsigned char ch = 0;
  char *data;
  const AUDIO_FORMAT *audio_data = this->m_audio_input->get_audio_data();
  unsigned long audio_size = this->m_audio_input->get_audio_data_size();
//  const AUDIO_FORMAT *audio_data = (AUDIO_FORMAT*)audio->data();
  unsigned long pixel;

  long left_border = 0;
//  static long last_left_border = -surface->w;
  unsigned long read_cursor = 0;
  int pixels_per_second = 250;
  int frames_per_pixel = SAMPLE_RATE / pixels_per_second;

  read_cursor = this->m_audio_input->get_read_cursor();
  left_border = (long)(read_cursor - frames_per_pixel * surface->w);
  if (left_border < 0 )
    left_border = 0;

  bpp = surface->format->BytesPerPixel;
  data = (char*)surface->pixels;
  SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 20, 100, 20));
//  SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, ch, ch, ch));
  ch++;
  this->m_audio_input->lock_audio_writing();
    for (int ind = 0;
         ind < surface->w && ind * frames_per_pixel + left_border < audio_size;
         ind++)
    {
      y1 = (int)((double)(max(audio_data + ind*frames_per_pixel + left_border, frames_per_pixel) / pow(256, sizeof(AUDIO_FORMAT)) * surface->h + 0.5f * (double)surface->h));
      y2 = (int)((double)(min(audio_data + ind*frames_per_pixel + left_border, frames_per_pixel) / pow(256, sizeof(AUDIO_FORMAT)) * surface->h + 0.5f * (double)surface->h));
//      pixel = SDL_MapRGB(surface->format, 255, 255, 0);
      if (y1 < 0 ) y1 = 0;
      if (y1 >= surface->h) y1 = surface->h - 1;
      if (y2 < 0 ) y2 = 0;
      if (y2 >= surface->h) y2 = surface->h - 1;
      rect.x = ind;
      rect.w = 1;
      rect.y = y2;
      rect.h = y1 - y2 + 1;
      SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 255, 255, 0));
//      this->DrawVLine(ind, y2, y1 - y2 + 1, PG_Color(255, 255, 0));
    }
    this->m_audio_input->unlock_audio_writing();
    
    //*/
/*
    // Try to lock the surface.
    if (SDL_LockSurface(surface) != 0) {
      fprintf(stderr, "WidgetSpectrum::update couldn't lock surface.\n");
      return;
    }
    
    for (int ind = 0;
         ind < surface->w && ind * frames_per_pixel + left_border < audio_size;
         ind++) {
      y1 = (int)((double)(max(audio_data + ind*frames_per_pixel + left_border, frames_per_pixel) / pow(256, sizeof(AUDIO_FORMAT)) * surface->h + 0.5f * (double)surface->h));
      y2 = (int)((double)(min(audio_data + ind*frames_per_pixel + left_border, frames_per_pixel) / pow(256, sizeof(AUDIO_FORMAT)) * surface->h + 0.5f * (double)surface->h));
      pixel = SDL_MapRGB(surface->format, 255, 255, 0);
      if (y1 < 0 ) y1 = 0;
      if (y1 >= surface->h) y1 = surface->h - 1;
      if (y2 < 0 ) y2 = 0;
      if (y2 >= surface->h) y2 = surface->h - 1;
      for (unsigned int jnd = 0; jnd < bpp; jnd++) {
        data[bpp*(ind+surface->w*y1)+jnd] = ((char*)&pixel)[jnd];
        data[bpp*(ind+surface->w*y2)+jnd] = ((char*)&pixel)[jnd];
      }
    }
  //  delete pixel;
//    ch++;
  
    SDL_UnlockSurface(surface);
//  }
  this->Update(true);
 //*/
  
//  this->Blit();
}
