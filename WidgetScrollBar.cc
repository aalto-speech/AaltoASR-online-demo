
#include "WidgetScrollBar.hh"

WidgetScrollBar::WidgetScrollBar(PG_Widget *parent,
                                 const PG_Rect &rect,
                                 ScrollDirection direction,
                                 int id,
                                 const std::string &style)
  : PG_ScrollBar(parent, rect, direction, id, style.data())
{
}

void
WidgetScrollBar::SetRange(Uint32 min, Uint32 max)
{
  PG_ScrollBar::SetRange(min, max);
  // TODO: Only need to set the size (and position) of dragbutton....
  this->RecalcPositions();
}

void
WidgetScrollBar::RecalcPositions()
{
  Uint16 length, thickness;
  
  if(this->sb_direction == VERTICAL) {
    length = this->my_height;
    thickness = this->my_width;
  }
  else {
    length = this->my_width;
    thickness = this->my_height;
  }

  this->position[0].x = 0;
  this->position[0].y = 0;
  this->position[0].w = this->position[0].h = thickness;
  this->position[1].w = this->position[1].h = thickness;
  
  // Overwrite some default calculations to create the behaviour we want:
  // dragbutton resizes relative to page_size.
  // Code was copy-pasted and modified from pgscrollbar.cpp.
  if(this->sb_direction == VERTICAL) {
    this->position[1].x = 0;
    this->position[1].y = abs(length - thickness);

    this->position[2].x = 0;
    this->position[2].y = thickness;
    this->position[2].w = thickness;
    this->position[2].h = abs(length - (thickness * 2));

    this->position[3].x = 0;
    this->position[3].w = thickness;

    this->calculate_position_and_size(this->position[2].y,
                                      this->position[2].h,
                                      this->position[3].y,
                                      this->position[3].h);
  }
  else {
    this->position[1].x = abs(length - thickness);
    this->position[1].y = 0;

    this->position[2].x = thickness;
    this->position[2].y = 0;
    this->position[2].w = abs(length - (thickness * 2));
    this->position[2].h = thickness;

    this->position[3].y = 0;
    this->position[3].h = thickness;
    
    this->calculate_position_and_size(this->position[2].x,
                                      this->position[2].w,
                                      this->position[3].x,
                                      this->position[3].w);
  }
  
  // Calculate borders (for scrollbuttons both borders).
  for(int ind = 0; ind < 4; ind++) {
    if(this->sb_direction == VERTICAL || ind < 2) {
      this->calculate_borders(this->position[ind].x, this->position[ind].w);
    }
    if(this->sb_direction == HORIZONTAL || ind < 2) {
      this->calculate_borders(this->position[ind].y, this->position[ind].h);
    }
  }
  
  // Set the positions of the scroll buttons.
  for (int ind = 0; ind < 2; ind++) {
    if (this->scrollbutton[ind])
      this->scrollbutton[ind]->MoveWidget(this->position[ind], false);
  }

  // Resize and move the drag button, but do NOT update screen content.    
  this->dragbutton->MoveWidget(this->position[3], false);
}

void
WidgetScrollBar::calculate_position_and_size(Sint16 start_pos,
                                             Uint16 total_size,
                                             Sint16 &position,
                                             Uint16 &size)
{
  double scroll_size = this->scroll_max - this->scroll_min;
  double relative_size = this->my_pagesize / (scroll_size + this->my_pagesize);
  double relative_scroll = this->scroll_current / scroll_size;
  
  size = (Uint16)(relative_size * total_size);
  position = start_pos;
  if(scroll_size > 0) {
    position += (Sint16)(relative_scroll * (total_size - size));
  }

}

void
WidgetScrollBar::calculate_borders(Sint16 &position, Uint16 &size)
{
  position += this->my_bordersize;
  if(size > 2 * this->my_bordersize) {
    size -= 2 * this->my_bordersize;
  }
}
