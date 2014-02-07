
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
  
  /** Constructs a new scrolling audio view.
   * \param parent Parent widget.
   * \param rect Location in the parent.
   * \param audio_input Source of the audio data.
   * \param pixels_per_second Adjusts the resolution. */
  WidgetAudioView(PG_Widget *parent,
                  const PG_Rect &rect,
                  AudioInputController *audio_input,
                  unsigned int pixels_per_second);
  /** Destructs the audio view. */
  virtual ~WidgetAudioView();
  
  /** Resets the audio view. Clears the view. */
  void reset();
  /** Updates the view using the audio data source. */
  void update();
  
  /** \param pos The position of the left side of the view. */
  inline void set_scroll_position(unsigned long pos);
  
  /** Initializes the view. Must be called once before any updating or being
   * visible. */
  virtual void initialize();
  /** Terminates the view. May be called after updates are no longer used and
   * the view is not visible anymore. */
  virtual void terminate();
  
protected:

  /** Calculates the area that can be blitted from previously drawn surface.
   * \param oldview_from X coordinate of the blitting area in the previous view
   *                     is calculated into this parameter.
   * \param oldview_to X coordinate of the blitting area in the new view is
   *                   calculated into this parameter.
   * \param oldview_size Size of the area to blit is calculated into this. */
  void calculate_old(unsigned int &oldview_from,
                     unsigned int &oldview_to,
                     unsigned int &oldview_size);

  /** Fixes the old blittable area by reducing area that wasn't actually
   * drawn yet (or was drawn incorrectly, e.g. wasn't enough data).
   * \param oldview_from X coordinate of the blitting area in the previous view.
   * \param oldview_size Size of the area to blit is fixed. */
  virtual void fix_oldview_size(unsigned int oldview_from,
                                unsigned int &oldview_size);

  /** Blits the old view by first calculating and fixing the old view and
   * then actually doing the blit.
   * \return The size of the blitted area, that is, the size of the part of the
   *         previous view that is visible also in the new view. */
  unsigned int blit_old();

  /** Calculates the area that should be drawn and that has audio data to
   * draw.
   * \param old_size Size of the area which can be blitted from backbuffer.
   * \param newaudio_from Sample where the drawing should begin is stored here.
   * \param newaudio_size Number of samples to draw is stored here. */
  void calculate_new(unsigned long old_size,
                     unsigned long &newaudio_from,
                     unsigned long &newaudio_size);

  /** Draws the new area by drawing pixel wide vectors.
   * \param x Screen coordinate of the view where to start the drawing.
   * \param newaudio_from Sample where the drawing should begin.
   * \param newaudio_size Number of samples to draw. */
  void draw_new(unsigned int x,
                unsigned long newaudio_from,
                unsigned long newaudio_size);

  /** Draws a pixel vector.
   * \param surface */
  virtual void draw_screen_vector(SDL_Surface *surface, unsigned int x) = 0;
  
  virtual bool eventMouseButtonUp(const SDL_MouseButtonEvent *button);
  
  inline void set_background_color(Uint32 color);

  AudioInputController *m_audio_input; //!< Source of audio data.
  /** Updated view is stored here so it can be used in next update if the
   * next view position collides with the previous. */
  SDL_Surface *m_surface_backbuffer; 
  
  const unsigned int m_pixels_per_second; //!< Time resolution.
  const double m_samples_per_pixel; //!< Audio samples per pixel.
  
  unsigned long m_scroll_pos; //!< Current scroll position.
  unsigned long m_last_scroll_pos; //!< Scroll position in last update.
  unsigned long m_last_audio_data_size; //!< Audio data size in last update.
  
  bool m_force_redraw; //!< Force redrawing of the entire view.
  
private:  
  
  Uint32 m_background_color; //!< Background color.

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
