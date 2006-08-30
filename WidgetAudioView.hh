
#ifndef WIDGETAUDIOVIEW_HH_
#define WIDGETAUDIOVIEW_HH_

#include <pgwidget.h>
#include "AudioInputController.hh"

/** Class for a scrolling view of audio. This abstract class doesn't know how
 * the audio is actually drawn, it just handles the scrolling by blitting
 * previously drawn surface and asking to redraw some screen parts when
 * necessary. */
class WidgetAudioView  :  public PG_Widget
{

public:
  
  WidgetAudioView(PG_Widget *parent,
                  const PG_Rect &rect,
                  AudioInputController *audio_input,
                  unsigned int pixels_per_second);
  virtual ~WidgetAudioView();
  
  void reset();
  void update();
  
  inline void set_scroll_position(unsigned long pos);
  
  virtual void initialize();
  virtual void terminate();
  
//  inline void force_clear();
  
protected:

  /** Calculates the area that can be blitted from previously drawn surface. */
  void calculate_old(unsigned int &oldview_from,
                     unsigned int &oldview_to,
                     unsigned int &oldview_size);

  /** Fixes the old blittable area by reducing area that wasn't actually
   * drawn yet (or was draw incorrectly). */
  virtual void fix_oldview_size(unsigned int oldview_from,
                                unsigned int &oldview_size);

  /** Blits the old view by first calculating and fixing the old view and
   * then actually doing the blit. */
  unsigned int blit_old();

  /** Calculates the area that should be drawn and that has audio data to
   * draw. */
  void calculate_new(unsigned long old_size,
                     unsigned long &newaudio_from,
                     unsigned long &newaudio_size);

  /** Draws the new area by drawing pixel wide vectors. */
  void draw_new(unsigned int x,
                unsigned long newaudio_from,
                unsigned long newaudio_size);

  /** Draws a pixel vector. */
  virtual void draw_screen_vector(SDL_Surface *surface, unsigned int x) = 0;
  
  virtual bool eventMouseButtonUp(const SDL_MouseButtonEvent *button);
  
  inline void set_background_color(Uint32 color);

  AudioInputController *m_audio_input;
  SDL_Surface *m_surface_backbuffer;
  
  const unsigned int m_pixels_per_second;
  const double m_samples_per_pixel;
  
  unsigned long m_scroll_pos;
  unsigned long m_last_scroll_pos;
  unsigned long m_last_audio_data_size;
  
  bool m_force_redraw;
  
private:  
  
  Uint32 m_background_color;

};

void
WidgetAudioView::set_scroll_position(unsigned long pos)
{
  this->m_scroll_pos = pos;
}

void
WidgetAudioView::set_background_color(Uint32 color)
{
  this->m_background_color = color;
}

#endif /*WIDGETAUDIOVIEW_HH_*/
