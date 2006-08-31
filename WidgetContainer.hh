
#ifndef WIDGETCONTAINER_HH_
#define WIDGETCONTAINER_HH_

#include <pgwidget.h>

/** This class is used to create word containers in the scrolling hypothesis
 * view. This way all morphemes are in the same unite rectangle and it also
 * enables the possibility to force word breaks. */
class WidgetContainer  :  public PG_Widget
{
public:
  WidgetContainer(PG_Widget *parent, Sint16 x, Sint16 y, const PG_Color &background);
  virtual ~WidgetContainer() { };
  
  virtual void AddChild(PG_Widget *item);
  virtual bool RemoveChild(PG_Widget *item);

private:
  void resize(Uint16 width, Uint16 height);
  void calculate_size();
  Uint32 m_background_color;
};

#endif /*WIDGETCONTAINER_HH_*/
