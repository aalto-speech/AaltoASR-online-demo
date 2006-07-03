
#include "WidgetRecognitionArea.hh"

WidgetRecognitionArea::WidgetScrollArea(PG_Widget *parent, const PG_Rect &rect)
  : PG_Widget(parent, rect, false)
{
  this->m_spectrum = NULL;
  
  this->m_scroll_bar = new PG_ScrollBar(this,
                                        PG_Rect(0, this->Height()-21, this->Width(), 10),
                                        PG_ScrollBar::HORIZONTAL);
}

WidgetRecognitionArea::~WidgetScrollArea()
{
  delete this->m_spectrum;
  delete this->m_scroll_bar;
}



