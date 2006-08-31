
#ifndef WIDGETWAVE_HH_
#define WIDGETWAVE_HH_

#include "WidgetAudioView.hh"

/** A scrollable view of the audio wave. */
class WidgetWave  :  public WidgetAudioView
{

public:
  
  WidgetWave(PG_Widget *parent, const PG_Rect &rect,
             AudioInputController *audio_input, unsigned int pixels_per_second);
  virtual ~WidgetWave() { };
  
  virtual void initialize();

protected:

  virtual void draw_screen_vector(SDL_Surface *surface, unsigned int x);

private:
  
  Uint32 m_foreground_color;

};

#endif /*WIDGETWAVE_HH_*/
