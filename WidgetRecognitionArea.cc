
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
  pthread_mutex_init(&this->m_scroll_lock, NULL);

  // Create spectrogram.
  this->m_spectrum = new WidgetSpectrum(this,
                                        PG_Rect(0, 0, this->Width(), (int)(0.3 * this->Height())),
                                        audio_input,
                                        pixels_per_second);
  this->AddChild(this->m_spectrum);
  
  // Create recognition text area.
  this->m_text_area = new WidgetRecognitionTexts(this,
                                                 PG_Rect(0, (int)(0.4 * this->Height()), this->Width(), (int)(0.4 * this->Height())),
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
  delete this->m_spectrum;
  delete this->m_text_area;
  delete this->m_scroll_bar;
}
/*
void
WidgetRecognitionArea::initialize()
{
  
}

void
WidgetRecognitionArea::terminate()
{
  delete this->m_spectrum;
  delete this->m_scroll_bar;
}
//*/

void
WidgetRecognitionArea::reset()
{
  this->m_spectrum->reset();
  this->m_text_area->reset();
  this->set_scroll_range();
  this->m_scroll_bar->SetPosition(0);
}

void
WidgetRecognitionArea::update()
{
  pthread_mutex_lock(&this->m_scroll_lock);
  this->set_scroll_range();

  // Update left border.
  if (this->m_autoscroll) {
    unsigned long read_cursor_px = this->m_audio_input->get_read_cursor() / this->m_frames_per_pixel;
    unsigned long scroll_pos = 0;
    if (read_cursor_px > this->Width())
      scroll_pos = read_cursor_px - this->Width();
      
    this->m_scroll_bar->SetPosition(scroll_pos);
    this->m_text_area->set_scroll_position(scroll_pos);
    this->m_spectrum->set_position(scroll_pos);
  }
  else {
    // This will do redrawing for scroll bar. (Update and Redraw won't work properly..)
    this->m_scroll_bar->SetPosition(this->m_scroll_bar->GetPosition());
  }

  this->m_spectrum->update();
  this->m_text_area->update();
  pthread_mutex_unlock(&this->m_scroll_lock);
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
//  fprintf(stderr, "Scrolling event: %d\n", page);
  if (!this->m_autoscroll) {
    pthread_mutex_lock(&this->m_scroll_lock);
    this->m_spectrum->set_position(page);
    // TODO: THREAD SAFETY!!!!!!!!!
    this->m_spectrum->update();
    this->m_text_area->set_scroll_position(page);
    pthread_mutex_unlock(&this->m_scroll_lock);
  }
  return true;
}


