
#include "WidgetRecognitionTexts.hh"

WidgetRecognitionTexts::WidgetRecognitionTexts(PG_Widget *parent,
                                               const PG_Rect &rect,
                                               Recognition *recognition,
                                               unsigned int pixels_per_second)
  : WidgetScrollArea(parent, rect),//PG_ScrollArea(parent, rect),
    m_pixels_per_second(pixels_per_second)
{
  this->m_recognition = recognition;
  this->initialize();
}

WidgetRecognitionTexts::~WidgetRecognitionTexts()
{
  this->terminate();
}

void
WidgetRecognitionTexts::initialize()
{
  this->m_last_morpheme_count = 0;
}

void
WidgetRecognitionTexts::terminate()
{
  for (unsigned int i = 0; i < this->m_morpheme_buttons.size(); i++) {
//    this->remove_child(this->m_morpheme_buttons.at(i));
    delete this->m_morpheme_buttons.at(i);
  }
  this->remove_all_childs();
  this->m_morpheme_buttons.clear();
}
  
void
WidgetRecognitionTexts::update()
{
  const std::vector<Morpheme> *morphemes = this->m_recognition->get_morphemes();
  unsigned int morpheme_count = morphemes->size();
  if (morpheme_count > this->m_last_morpheme_count) {
    for (unsigned int ind = this->m_last_morpheme_count; ind < morpheme_count; ind++) {
      this->add_morpheme_button(&morphemes->at(ind));
    }
    this->m_last_morpheme_count = morpheme_count;
//    this->Redraw(true);
//    this->Update(true);
  }
}

void
WidgetRecognitionTexts::reset()
{
  this->terminate();
//  this->set_scroll_position(0);
  this->initialize();
}

void
WidgetRecognitionTexts::add_morpheme_button(const Morpheme *morpheme)
{
//  this->SetAreaWidth(20000);
  double multiplier = this->m_pixels_per_second / (double)Recognition::frames_per_second;
  Sint32 x = (Sint32)(morpheme->time * multiplier);
  unsigned int w = (int)(morpheme->duration * multiplier);
  PG_Rect rect(0, 0, w, 30);
  PG_Label *button = new PG_Label(this, rect, morpheme->data.data());
  fprintf(stderr, "morpheme button x: %d, data: %s\n", x, morpheme->data.data());
  this->add_child(button, x);
  button->SetAlignment(PG_Label::CENTER);
  this->m_morpheme_buttons.push_back(button);
//  this->SetAreaWidth(20000);
}
