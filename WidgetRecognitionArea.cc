
#include "WidgetRecognitionArea.hh"

WidgetRecognitionArea::WidgetRecognitionArea(PG_Widget *parent,
                                             const PG_Rect &rect,
                                             AudioInputController *audio_input,
                                             Recognition *recognition,
                                             unsigned int pixels_per_second)
  : PG_Widget(parent, rect, false),
    m_pixels_per_second(pixels_per_second),
    m_frames_per_pixel(SAMPLE_RATE / pixels_per_second)
{
  this->m_audio_input = audio_input;
  this->m_autoscroll = false;
//  this->m_autoscroll = DISABLE;
  pthread_mutex_init(&this->m_scroll_lock, NULL);

  // Create wave view.
  this->m_wave = new WidgetWave(this,
                                PG_Rect(0, 0, this->Width(), (int)(0.3 * this->Height())),
                                audio_input,
                                pixels_per_second);
  this->m_wave->initialize();
  this->AddChild(this->m_wave);
  
  // Create spectrogram.
  this->m_spectrogram = new WidgetSpectrogram(this,
                                              PG_Rect(0,
                                                      (int)(0.3 * this->Height()),
                                                      this->Width(),
                                                      (int)(0.4 * this->Height())),
                                              audio_input,
                                              pixels_per_second);
  this->m_spectrogram->initialize();
  this->AddChild(this->m_spectrogram);
  
  // Create recognition text area.
  this->m_text_area = new WidgetRecognitionTexts(this,
                                                 PG_Rect(0,
                                                         (int)(0.75 * this->Height()),
                                                         this->Width(),
                                                         (int)(0.2 * this->Height())),
                                                 recognition,
                                                 pixels_per_second);
  this->AddChild(this->m_text_area);
  
  // Create scroll bar.
  this->m_scroll_bar = new PG_ScrollBar(this,
                                        PG_Rect(0, this->Height()-21, this->Width(), 10),
                                        PG_ScrollBar::HORIZONTAL);
  this->AddChild(this->m_scroll_bar);
  this->m_scroll_bar->SetPageSize(this->Width());
  this->m_scroll_bar->SetLineSize(30);
  this->set_scroll_range();
  this->m_scroll_bar->sigScrollPos.connect(slot(*this, &WidgetRecognitionArea::handle_scroll));
  this->m_scroll_bar->sigScrollTrack.connect(slot(*this, &WidgetRecognitionArea::handle_scroll));
//  fprintf(stderr, "after\n");
}

WidgetRecognitionArea::~WidgetRecognitionArea()
{
  pthread_mutex_destroy(&this->m_scroll_lock);
  this->m_wave->terminate();
  this->m_spectrogram->terminate();
  delete this->m_wave;
  delete this->m_spectrogram;
  delete this->m_text_area;
  delete this->m_scroll_bar;
}

void
WidgetRecognitionArea::reset()
{
  pthread_mutex_lock(&this->m_scroll_lock);
  // Scroll to zero.
  this->set_scroll_position(0);
  // Reset components.
  this->m_wave->reset();
  this->m_spectrogram->reset();
  this->m_text_area->reset();
  this->set_scroll_range();
  pthread_mutex_unlock(&this->m_scroll_lock);
}

void
WidgetRecognitionArea::set_scroll_position(unsigned long page)
{
  this->m_scroll_bar->SetPosition(page);
  this->m_text_area->set_scroll_position(page);
  this->m_wave->set_position(page);
  this->m_spectrogram->set_position(page);
}

void
WidgetRecognitionArea::update_screen(bool new_data)
{
//  fprintf(stderr, "WRA:update_screen start\n");
  this->m_wave->update();
  this->m_spectrogram->update();
  if (new_data)
    this->m_text_area->update();
//  fprintf(stderr, "WRA:update_screen before\n");
  this->Update(true);
//  fprintf(stderr, "WRA:update_screen after\n");
//  fprintf(stderr, "WRA:update_screen end\n");
}

void
WidgetRecognitionArea::update()
{
//  fprintf(stderr, "WRA:update start\n");
  pthread_mutex_lock(&this->m_scroll_lock);
  this->set_scroll_range();

//  fprintf(stderr, "WRA:update before\n");
  // Update left border.
  if (this->m_autoscroll) {
    unsigned long read_cursor_px = this->m_audio_input->get_audio_cursor() / this->m_frames_per_pixel;
    unsigned long scroll_pos = 0;
    if (read_cursor_px > this->Width())
      scroll_pos = read_cursor_px - this->Width();

    this->set_scroll_position(scroll_pos);      
  }
  else {
    // This will do redrawing for scroll bar. (Update and Redraw won't work properly..)
    this->m_scroll_bar->SetPosition(this->m_scroll_bar->GetPosition());
  }
//  fprintf(stderr, "WRA:update after\n");

  this->update_screen(true);
  pthread_mutex_unlock(&this->m_scroll_lock);
//  fprintf(stderr, "WRA:update end\n");
}

void
WidgetRecognitionArea::set_scroll_range()
{
  unsigned long audio_data_pixels = this->m_audio_input->get_audio_data_size() / this->m_frames_per_pixel;
  if (audio_data_pixels > this->Width()) {
    this->m_scroll_bar->SetRange(0, audio_data_pixels - this->Width());
  }
  else {
    this->m_scroll_bar->SetRange(0, 0);
  }
}

bool
WidgetRecognitionArea::handle_scroll(PG_ScrollBar *scroll_bar, long page)
{
  if (!this->m_autoscroll) {
    pthread_mutex_lock(&this->m_scroll_lock);
    this->set_scroll_position(page);
//    this->update_screen(false);
    pthread_mutex_unlock(&this->m_scroll_lock);
  }
  return true;
}


