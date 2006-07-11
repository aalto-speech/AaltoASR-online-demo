
#ifndef WIDGETWAVE_HH_
#define WIDGETWAVE_HH_

//#include <pthread.h>
#include <pgwidget.h>
#include "AudioInputController.hh"

class WidgetWave  :  public PG_Widget
{

public:
  
  WidgetWave(PG_Widget *parent, const PG_Rect &rect,
             AudioInputController *audio_input, unsigned int pixels_per_second);
  virtual ~WidgetWave();
  
  void reset();
  void update();
  
  inline void set_position(unsigned long pos) { this->m_left_index = pos; }
//  inline void force_redraw() { this->m_force_redraw = true; }
  // TODO: EI FORCE, VAAN PITÄÄ MUISTISSA JOTAIN OIKEAA LAITAA JOHON ASTI
  // PIIRTÄNYT! EI OLETA ETTÄ KOKO NÄKYMÄN ALA ON PÄIVITETTY JOS ON KERRAN
  // PIIRRETTY!!!!
  
  virtual void initialize();
  virtual void terminate();
  
protected:

  unsigned int blit_old(unsigned long left_index);
  unsigned long calculate_new(unsigned long left_index, unsigned long old_size);
  virtual void draw_new(unsigned long left_index, unsigned int oldview_size, unsigned int audio_size);

  AudioInputController *m_audio_input;
  SDL_Surface *m_surface_backbuffer;
  AUDIO_FORMAT *m_audio_buffer;
  const unsigned int m_pixels_per_second;
  const unsigned int m_frames_per_pixel;
  unsigned long m_left_index, m_last_left_index;
  unsigned int m_width, m_height;
  unsigned long m_last_audio_data_size;
  bool m_force_redraw;
  Uint32 m_foreground_color;
  Uint32 m_background_color;

};

#endif /*WIDGETWAVE_HH_*/
