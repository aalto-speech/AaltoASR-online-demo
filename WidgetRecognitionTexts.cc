
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
  double multiplier = this->m_pixels_per_second / (double)Recognition::frames_per_second;
  Sint32 x = (Sint32)(morpheme->time * multiplier);
  unsigned int w = (int)(morpheme->duration * multiplier);
  PG_Rect rect;
  PG_Widget *button;

  if (morpheme->data == "<w>") {
    rect.SetRect(0, 0, w > 1 ? w : 1, 30);
    button = new PG_Widget(this, rect, true);
    this->add_child(button, x);
    SDL_FillRect(button->GetWidgetSurface(),
                 NULL,
                 button->GetFontColor().MapRGB(button->GetWidgetSurface()->format));
/*    
    rect.SetRect(0, 0, 1, 30);
    button = new PG_Widget(this, rect, true);
    this->add_child(button, x + w / 2);
    SDL_FillRect(button->GetWidgetSurface(),
                 NULL,
                 button->GetFontColor().MapRGB(button->GetWidgetSurface()->format));
//*/
    //button->DrawVLine(0, 0, 30, PG_Color(255, 255, 255));
  }
  else {
    rect.SetRect(0, 0, w, 30);
    button = new PG_Label(this, rect, morpheme->data.data());
    ((PG_Label*)button)->SetAlignment(PG_Label::CENTER);
//    button = new PG_Button(this, rect, morpheme->data.data());
    this->add_child(button, x);
  }

//  fprintf(stderr, "morpheme button x: %d, data: %s\n", x, morpheme->data.data());
  this->m_morpheme_buttons.push_back(button);
}
