
#include "WidgetRecognitionText.hh"
#include "WidgetContainer.hh"
//#include <pgscrollarea.h>
#include <pglabel.h>

WidgetRecognitionText::WidgetRecognitionText(PG_Widget *parent,
                                               const PG_Rect &rect,
                                               AudioInputController *audio_input,
                                               RecognizerStatus *recognition,
                                               unsigned int pixels_per_second)
  : WidgetScrollArea(parent, rect),
    m_pixels_per_second(pixels_per_second)
{
  this->m_audio_input = audio_input;
  this->m_recognition = recognition;
  this->initialize();
}

WidgetRecognitionText::~WidgetRecognitionText()
{
  this->terminate();
}

void
WidgetRecognitionText::initialize()
{
  this->m_last_recognition_count = 0;
  this->m_last_recog_word = NULL;
  this->m_last_recog_morph = NULL;
  this->m_last_recognition_frame = 0;

//  this->m_recognition->lock();
//  this->m_recognized_iter = this->m_recognition->get_recognized().begin();
//  this->m_recognition->unlock();
}

void
WidgetRecognitionText::terminate()
{
  this->RemoveAllChilds();
  this->m_hypothesis_widgets.clear();
}

void
WidgetRecognitionText::update()
{
  this->m_recognition->lock();
  unsigned long frame = this->m_recognition->get_recognition_frame();
  if (frame > this->m_last_recognition_frame) {
    this->update_recognition();
    this->update_hypothesis();
    this->m_last_recognition_frame = frame;
  }
  this->m_recognition->unlock();
}

void
WidgetRecognitionText::reset()
{
  this->terminate();
  this->initialize();
}

PG_Widget*
WidgetRecognitionText::add_morpheme_widget(const Morpheme &morpheme,
                                           const PG_Color &color,
                                           PG_Widget *&word_widget,
                                           Sint32 min_x)
{
  // Calculate recognizer pixels per recognizer frame.
  double multiplier = this->m_pixels_per_second /
                      (double)RecognizerStatus::frames_per_second;
                      
  unsigned int w = (int)(morpheme.duration * multiplier);
  Sint32 x = (Sint32)(morpheme.time * multiplier);
  
  // Word break.
  if (morpheme.data == " " || morpheme.data == "." || morpheme.data == "")
    word_widget = NULL;

  //* min_x is used to force word breaks.
  // These lines may be commented out if forcing is undesired. Note that
  // then sentence breaks will most likely hide.
  if (x < min_x) {
    // Cut width.
    if ((Sint32)w + x > min_x)
      w = (long)(w + x) - min_x;
    else
      w = 1;
    // Set new position.
    x = min_x;
  }
  //*/

  PG_Rect rect(0, 0, w, this->my_height);
  PG_Widget *morph_widget = NULL;

  if (morpheme.data != " " && morpheme.data != "") {
    if (morpheme.data == ".") {
      // One pixel wide separator for sentence breaks.
      rect.w = 1;
      morph_widget = new PG_Widget(NULL, rect, true);
      SDL_FillRect(morph_widget->GetWidgetSurface(),
                   NULL,
                   color.MapRGB(morph_widget->GetWidgetSurface()->format));
      morph_widget->SetUserData(&x, sizeof(Sint32));
      this->AddChild(morph_widget);
//      morph_widget->BringToFront();
      morph_widget->SetVisible(true);
    }
    else {
      if (word_widget == NULL) {
        word_widget = new WidgetContainer(NULL, 0, 0, PG_Color(90, 90, 90));
        word_widget->SetUserData(&x, sizeof(Sint32));
        this->AddChild(word_widget);
        word_widget->SetVisible(true);
      }
      /*
      if (word_widget == NULL) {
        word_widget = new PG_Widget(NULL, rect, true);
        word_widget->SetUserData(&x, sizeof(Sint32));
        this->AddChild(word_widget);
      }
      //*/

      //*
      Sint32 word_x;
      word_widget->GetUserData(&word_x);
      rect.x = x - word_x;
      morph_widget = new PG_Label(word_widget, rect, morpheme.data.data());
      //*/
      /*
      morph_widget = new PG_Label(NULL, rect, morpheme.data.data());
      morph_widget->SetUserData(&x, sizeof(Sint32));
      this->AddChild(morph_widget);
      //*/
      ((PG_Label*)morph_widget)->SetAlignment(PG_Label::CENTER);
      morph_widget->SetFontColor(color);
      morph_widget->sigMouseButtonUp.connect(slot(*this, &WidgetRecognitionText::handle_morpheme_widget), NULL);
      morph_widget->SetVisible(true);
      
      /*
      fprintf(stderr, "add morph 3.1\n");
      Sint32 word_x;
      word_widget->GetUserData(&word_x);
      word_widget->SizeWidget(x + rect.w - word_x,
                              word_widget->my_height,
                              false);
      SDL_FillRect(word_widget->GetWidgetSurface(),
                   NULL,
                   SDL_MapRGB(word_widget->GetWidgetSurface()->format, 90, 90, 90));
      word_widget->SendToBack();
      word_widget->SetVisible(true);
      fprintf(stderr, "add morph 4\n");
      //*/
    }
  }  
  
  return morph_widget;
}

    /* OLDER. HERE SENTENCE BREAKS ARE IGNORED AND WORD BREAKS ARE BLOCKS.
    if (morpheme.data == " ") {
      rect.w = w > 1 ? w : 1; // Word separator at least one pixel wide.
      item = new PG_Widget(NULL, rect, true);
      SDL_FillRect(item->GetWidgetSurface(),
                   NULL,
                   color.MapRGB(item->GetWidgetSurface()->format));
    }
    else {
      item = new PG_Label(NULL, rect, morpheme.data.data());
      ((PG_Label*)item)->SetAlignment(PG_Label::CENTER);
      ((PG_Label*)item)->SetFontColor(color);
      item->sigMouseButtonUp.connect(slot(*this, &WidgetRecognitionText::handle_morpheme_widget), NULL);
    }
    //*/

void
WidgetRecognitionText::update_recognition()
{
  const MorphemeList &morphemes = this->m_recognition->get_recognized();
  Sint32 min_x = 0;
//  PG_Widget *last_nonnull_widget = this->m_last_recog_morph;
//  PG_Widget *previous_word = this->m_last_recog_word;
//  PG_Widget *current_morph = this->m_last_recog_morph;
  
  // Update if new morphemes has been recognized.
  if (morphemes.size() > this->m_last_recognition_count) {
    MorphemeList::const_iterator iter = morphemes.begin();
    
    // Find the position of last recognized morpheme.
    for (unsigned int ind = 0; ind < this->m_last_recognition_count; ind++)
      iter++;
      
    // Add new recognized morphemes.
    for (; iter != morphemes.end(); iter++) {
      
      // Force at least 2 pixels between words/separators.
      if (!this->m_last_recog_word && this->m_last_recog_morph) {
        this->m_last_recog_morph->GetUserData(&min_x);
        min_x += this->m_last_recog_morph->my_width + 2;
      }

      PG_Widget *current_morph =
        this->add_morpheme_widget(*iter,
                                  PG_Color(255, 255, 255),
                                  this->m_last_recog_word,
                                  min_x);
      
      // Save latest non-null widget (word/separator).
      if (this->m_last_recog_word)
        this->m_last_recog_morph = this->m_last_recog_word;
      else if (current_morph)
        this->m_last_recog_morph = current_morph;
        

    }
  }
  this->m_last_recognition_count = morphemes.size();
}

void
WidgetRecognitionText::update_hypothesis()
{//*
  PG_Widget *nonnull, *current_morph, *previous_word, *current_word;
  const MorphemeList &morphemes = this->m_recognition->get_hypothesis();
  Sint32 min_x = 0;

  this->remove_hypothesis();

  // Start where recognition ends.
//  previous_morph = NULL;
  nonnull = NULL;
  current_morph = this->m_last_recog_morph;
  current_word = this->m_last_recog_word;
  
  for (MorphemeList::const_iterator iter = morphemes.begin();
       iter != morphemes.end();
       iter++) {

    // Save latest non-null widget (word/separator).
    if (current_word)
      nonnull = current_word;
    else if (current_morph)
      nonnull = current_morph;

    if (!current_word && nonnull) {
      nonnull->GetUserData(&min_x);
      min_x += nonnull->my_width + 2;
    }

    previous_word = current_word;
    
    // Add the morpheme.
    current_morph = this->add_morpheme_widget(*iter,
                                              PG_Color(255, 255, 0),
                                              current_word,
                                              min_x);
    
    if (current_word == NULL) {
      // Possible sentence break widget.
      if (current_morph)
        this->m_hypothesis_widgets.push_back(current_morph);
    }
    else {
      if (current_word == this->m_last_recog_word) {
        // Hypothesis morpheme continues recognized word.
        if (current_morph)
          this->m_hypothesis_widgets.push_back(current_morph);
      }
      else if (current_word != previous_word) {
        // New word, completely hypothesis.
        this->m_hypothesis_widgets.push_back(current_word);
      }
    }
  }
}

void
WidgetRecognitionText::remove_hypothesis()
{
  for (unsigned int i = 0; i < this->m_hypothesis_widgets.size(); i++) {
    // Set visibility to false to fix the destructor bug which otherwise
    // updates the screen area.
    this->m_hypothesis_widgets.at(i)->SetVisible(false);
    delete this->m_hypothesis_widgets.at(i);
  }
  this->m_hypothesis_widgets.clear();
  
}

bool
WidgetRecognitionText::handle_morpheme_widget(PG_MessageObject *widget,
                                               const SDL_MouseButtonEvent *event,
                                               void *user_data)
{
  double multiplier = (double)SAMPLE_RATE / this->m_pixels_per_second;
  unsigned long x = ((PG_Widget*)widget)->my_xpos;
  unsigned long w = ((PG_Widget*)widget)->my_width;
  x += this->get_scroll_position();//this->GetScrollPosX();//
  x = (unsigned long)(x * multiplier);
  w = (unsigned long)(w * multiplier);
//  fprintf(stderr, "Morpheme pressed... Playing %d, %d.\n", x, w);
  this->m_audio_input->stop_playback();
  this->m_audio_input->start_playback(x, w);
  return true;
}

