
#include "WidgetTimeAxis.hh"
#include <pglabel.h>

WidgetTimeAxis::WidgetTimeAxis(PG_Widget *parent,
                               const PG_Rect &rect,
                               unsigned int pixels_per_second)
  : WidgetScrollArea(parent, rect),
    m_pixels_per_second(pixels_per_second)
{
  this->m_last_time = 0;
}

void
WidgetTimeAxis::add_time_label(unsigned int time)
{
  PG_Rect rect;
  char label[30];
  unsigned int seconds = time % 60;
  unsigned int minutes = time / 60;
  sprintf(label, "| %u:%02u", minutes, seconds);
  Uint32 x = (Uint32)this->m_pixels_per_second * time;
  
  PG_Label *time_label = new PG_Label(NULL, rect, label);
  time_label->SetSizeByText();
  time_label->SetAlignment(PG_Label::LEFT);
  time_label->SetUserData(&x, sizeof(unsigned int));
  this->AddChild(time_label);
  time_label->SetVisible(true);
}

void
WidgetTimeAxis::reset()
{
  this->RemoveAllChilds();
  this->m_last_time = 0;
}

void
WidgetTimeAxis::update(unsigned int time_length)
{
  if (time_length > this->m_last_time) {
    for (unsigned int ind = this->m_last_time + 1; ind <= time_length; ind++) {
      this->add_time_label(ind);
    }
    this->m_last_time = time_length;
  }
}
