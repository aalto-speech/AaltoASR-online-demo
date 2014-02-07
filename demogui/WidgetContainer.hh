
#ifndef WIDGETCONTAINER_HH_
#define WIDGETCONTAINER_HH_

#include <pgwidget.h>

/** This class is used to create word containers in the scrolling hypothesis
 * view. This way all morphemes are in the same unite rectangle and it also
 * enables the possibility to force word breaks. */
class WidgetContainer  :  public PG_Widget
{
public:
  /** Constructs a container widget.
   * \param parent Parent widget.
   * \param x X coordinate of the left border of the widget.
   * \param y Y coordinate of the top border of the widget.
   * \param background Background color of the container. */
  WidgetContainer(PG_Widget *parent,
                  Sint16 x,
                  Sint16 y,
                  const PG_Color &background);
  virtual ~WidgetContainer() { };
  
  /** Adds a child into the widget. Resizes so that all widgets fit in the
   * container.
   * \param item Widget to add to the container. */
  virtual void AddChild(PG_Widget *item);
  /** Removes a widget from the container. Resizes so that all widgets fit in
   * the container but isn't too big.
   * \param item Widget to remove. */
  virtual bool RemoveChild(PG_Widget *item);

private:
  /** Resizes the container. */
  void resize(Uint16 width, Uint16 height);
  /** Calculates and resizes the container so that all widgets fit in it but it
   * isn't oo big. */
  void calculate_size();
  Uint32 m_background_color; //!< Background color.
};

#endif /*WIDGETCONTAINER_HH_*/
