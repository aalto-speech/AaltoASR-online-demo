
#include "WidgetRecognitionTexts.hh"

WidgetRecognitionTexts::WidgetRecognitionTexts(PG_Widget *parent,
                                               const PG_Rect &rect,
                                               AudioInputController *audio_input,
                                               Recognition *recognition,
                                               unsigned int pixels_per_second)
  : WidgetScrollArea(parent, rect),//PG_ScrollArea(parent, rect),//
    m_pixels_per_second(pixels_per_second)
{
  this->m_audio_input = audio_input;
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
  this->m_last_recognition_count = 0;
}

void
WidgetRecognitionTexts::terminate()
{
  this->RemoveAllChilds();
  this->m_hypothesis_widgets.clear();
}

void
WidgetRecognitionTexts::update()
{
  this->m_recognition->lock();
  this->update_recognition();
  this->update_hypothesis();
  this->m_recognition->unlock();
}

void
WidgetRecognitionTexts::reset()
{
  this->terminate();
  this->initialize();
}

PG_Widget*
WidgetRecognitionTexts::add_morpheme_widget(const Morpheme &morpheme,
                                            const PG_Color &color)
{
  double multiplier = this->m_pixels_per_second / (double)Recognition::frames_per_second;
  unsigned int w = (int)(morpheme.duration * multiplier);
  PG_Rect rect(0, 0, w, 30);
  Sint32 x = (Sint32)(morpheme.time * multiplier);
  PG_Widget *item;
  
  if (morpheme.data == "<w>") {
    rect.w = w > 1 ? w : 1; // Word separator at least one pixel wide.
    item = new PG_Widget(this, rect, true);
    SDL_FillRect(item->GetWidgetSurface(),
                 NULL,
                 color.MapRGB(item->GetWidgetSurface()->format));
  }
  else {
    item = new PG_Label(this, rect, morpheme.data.data());
    ((PG_Label*)item)->SetAlignment(PG_Label::CENTER);
    ((PG_Label*)item)->SetFontColor(color);
    item->sigMouseButtonUp.connect(slot(*this, &WidgetRecognitionTexts::handle_morpheme_widget), NULL);
  }

  // For information about this user data, see WidgetScrollArea.hh!
  item->SetUserData(&x, sizeof(Sint32));
  item->SetVisible(true);
  return item;
}

void
WidgetRecognitionTexts::update_recognition()
{
  const std::vector<Morpheme> *morphemes = this->m_recognition->get_morphemes();
  unsigned int morpheme_count = morphemes->size();
  
  if (morpheme_count > this->m_last_recognition_count) {
    for (unsigned int ind = this->m_last_recognition_count; ind < morpheme_count; ind++) {
      this->add_morpheme_widget(morphemes->at(ind), PG_Color(255, 255, 255));
    }
    this->m_last_recognition_count = morpheme_count;
  }
}

void
WidgetRecognitionTexts::update_hypothesis()
{
  PG_Widget *widget;
  const std::vector<Morpheme> *morphemes;

  this->remove_hypothesis();
  morphemes = this->m_recognition->get_hypothesis().get_morphemes();

  for (unsigned int ind = 0; ind < morphemes->size(); ind++) {
    widget = this->add_morpheme_widget(morphemes->at(ind), PG_Color(255, 255, 0));
    this->m_hypothesis_widgets.push_back(widget);
  }
}

void
WidgetRecognitionTexts::remove_hypothesis()
{
  for (unsigned int i = 0; i < this->m_hypothesis_widgets.size(); i++) {
    this->RemoveChild(this->m_hypothesis_widgets.at(i));
    delete this->m_hypothesis_widgets.at(i);
  }
  this->m_hypothesis_widgets.clear();
  
}

bool
WidgetRecognitionTexts::handle_morpheme_widget(PG_MessageObject *widget, const SDL_MouseButtonEvent *event, void *user_data)
{
  double multiplier = SAMPLE_RATE / this->m_pixels_per_second;
  unsigned long x = ((PG_Widget*)widget)->my_xpos;
  unsigned long w = ((PG_Widget*)widget)->my_width;
  x += this->get_scroll_position();//this->GetScrollPosX();//
  x = (unsigned long)(x * multiplier);
  w = (unsigned long)(w * multiplier);
  fprintf(stderr, "Morpheme pressed... Playing %d, %d.\n", x, w);
  this->m_audio_input->play(x, w);
  return true;
}

