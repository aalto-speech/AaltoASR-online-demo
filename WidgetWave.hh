
#ifndef WIDGETWAVE_HH_
#define WIDGETWAVE_HH_

#include "WidgetAudioView.hh"

class WidgetWave  :  public WidgetAudioView
{

public:
  
  WidgetWave(PG_Widget *parent, const PG_Rect &rect,
             AudioInputController *audio_input, unsigned int pixels_per_second);
  virtual ~WidgetWave() { };
  
  virtual void initialize();

  /*
  void reset();
  void update();
  
  inline void set_position(unsigned long pos) { this->m_left_index = pos; }
  
  virtual void initialize();
  virtual void terminate();
  //*/
protected:

//  virtual Uint32 get_background_color(SDL_PixelFormat *format) const;
/*
  void calculate_old(unsigned int &oldview_from,
                     unsigned int &oldview_to,
                     unsigned int &oldview_size);

  virtual void fix_oldview_size(unsigned int oldview_from,
                                unsigned int &oldview_size);
                                
  unsigned int blit_old();

  void calculate_new(unsigned long old_size,
                     unsigned long &newaudio_from,
                     unsigned long &newaudio_size);

  void draw_new(unsigned int x,
                unsigned long newaudio_from,
                unsigned long newaudio_size);
//*/
  virtual void draw_screen_vector(SDL_Surface *surface, unsigned int x);
  /*
  virtual bool eventMouseButtonUp(const SDL_MouseButtonEvent *button);

  AudioInputController *m_audio_input;
  SDL_Surface *m_surface_backbuffer;
  const unsigned int m_pixels_per_second;
  const double m_samples_per_pixel;
  unsigned long m_left_index, m_last_left_index;
  unsigned long m_last_audio_data_size;
  bool m_force_redraw;
  Uint32 m_foreground_color;
  Uint32 m_background_color;
//*/

private:
  
  Uint32 m_foreground_color;

};

#endif /*WIDGETWAVE_HH_*/
