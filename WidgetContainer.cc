
#include "WidgetContainer.hh"

WidgetContainer::WidgetContainer(PG_Widget *parent,
                                 Sint16 x,
                                 Sint16 y,
                                 const PG_Color &background)
  : PG_Widget(parent, PG_Rect(x, y, 0, 0), true)
{
  this->m_background_color = background.MapRGB(this->GetWidgetSurface()->format);
}

void
WidgetContainer::AddChild(PG_Widget *item)
{
  PG_Widget::AddChild(item);
  this->calculate_size();
}

bool
WidgetContainer::RemoveChild(PG_Widget *item)
{
  if (PG_Widget::RemoveChild(item)) {
    this->calculate_size();
    return true;
  }
  return false;
}

void
WidgetContainer::calculate_size()
{
  long most_right = 0, most_bottom = 0;
  long right, bottom;
  PG_RectList *child_list = this->GetChildList();
  if (child_list) {
    PG_Widget *iter;
    for (iter = child_list->first(); iter != NULL; iter = iter->next()) {
      right = (iter->my_xpos - this->my_xpos) + iter->my_width;
      bottom = (iter->my_ypos - this->my_ypos) + iter->my_height;
      if (right > most_right)
        most_right = right;
      if (bottom > most_bottom)
        most_bottom = bottom;
    }
  }
  this->resize(most_right, most_bottom);
}

void
WidgetContainer::resize(Uint16 width, Uint16 height)
{
  this->SizeWidget(width, height, false);
  SDL_Surface *surface = this->GetWidgetSurface();
  if (surface)
    SDL_FillRect(surface, NULL, this->m_background_color);
}
