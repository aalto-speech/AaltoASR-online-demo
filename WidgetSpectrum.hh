
#ifndef WIDGETSPECTRUM_HH_
#define WIDGETSPECTRUM_HH_

#include <pgwidget.h>
#include "AudioInputController.hh"

class WidgetSpectrum  :  public PG_Widget
{

public:
  
  WidgetSpectrum(PG_Widget *parent, const PG_Rect &rect,
                 AudioInputController *audio_input, unsigned int pixels_per_second);
  ~WidgetSpectrum();
  
  void reset();
  void update();
//  inline void force_redraw() { this->m_force_redraw = true; }
  // TODO: EI FORCE, VAAN PITÄÄ MUISTISSA JOTAIN OIKEAA LAITAA JOHON ASTI
  // PIIRTÄNYT! EI OLETA ETTÄ KOKO NÄKYMÄN ALA ON PÄIVITETTY JOS ON KERRAN
  // PIIRRETTY!!!!
  
protected:

  void initialize();
  void terminate();

  AudioInputController *m_audio_input;
  SDL_Surface *m_surface_backbuffer;
  AUDIO_FORMAT *m_audio_buffer;
  const unsigned int m_pixels_per_second;
  const unsigned int m_frames_per_pixel;
  unsigned long m_left_index, m_last_left_index;
  unsigned int m_width, m_height;
  unsigned long m_last_audio_data_size;
//  bool m_force_redraw;

};

#endif /*WIDGETSPECTRUM_HH_*/
