
#ifndef WIDGETWAVE_HH_
#define WIDGETWAVE_HH_

#include "WidgetAudioView.hh"

/** A scrollable view of the audio wave. */
class WidgetWave  :  public WidgetAudioView
{

public:
  
  /** 
   * \param parent Parent widget.
   * \param rect Rectangle area of the widget.
   * \param audio_input Source of audio data.
   * \param pixels_per_second Time resolution. */
  WidgetWave(PG_Widget *parent,
             const PG_Rect &rect,
             AudioInputController *audio_input,
             unsigned int pixels_per_second);
  virtual ~WidgetWave() { };
  
  virtual void initialize();

protected:

  virtual void draw_screen_vector(SDL_Surface *surface, unsigned int x);

private:
  
  Uint32 m_foreground_color; //!< Color of the wave.

};

#endif /*WIDGETWAVE_HH_*/
