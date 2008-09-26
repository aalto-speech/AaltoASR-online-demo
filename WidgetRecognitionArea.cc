
#include "WidgetRecognitionArea.hh"
#include "WidgetScrollBar.hh"
#include "AudioStream.hh"

WidgetRecognitionArea::WidgetRecognitionArea(PG_Widget *parent,
                                             const PG_Rect &rect,
                                             AudioInputController *audio_input,
                                             RecognizerStatus *recognition,
                                             unsigned int pixels_per_second)
  : PG_Widget(parent, rect, false),
    m_pixels_per_second(pixels_per_second),
    m_frames_per_pixel((double)audio::audio_sample_rate / (double)pixels_per_second)
{
  this->m_audio_input = audio_input;
  this->m_recognition = recognition;

  // Create autoscrolling radio buttons.
  int x = 10;
  PG_Label *label = new PG_Label(this, PG_Rect(x,0,0,0), "Autoscrolling:");
  label->SetSizeByText();
  x += label->my_width + 30;
  PG_RadioButton *audioscroll_radio = new PG_RadioButton(this,
                                                 PG_Rect(x,0,0,0),
                                                 "Audio");
  audioscroll_radio->SetSizeByText();
  audioscroll_radio->sigClick.connect(slot(*this, &WidgetRecognitionArea::handle_radio), (void*)AUDIO);
  x += audioscroll_radio->my_width + 20;
  PG_RadioButton *recognizerscroll_radio = new PG_RadioButton(this,
                                                              PG_Rect(x,0,0,0),
                                                              "Recognizer",
                                                              audioscroll_radio);
  recognizerscroll_radio->SetSizeByText();
  recognizerscroll_radio->sigClick.connect(slot(*this, &WidgetRecognitionArea::handle_radio), (void*)RECOGNIZER);
  x += recognizerscroll_radio->my_width + 20;
  this->m_disablescroll_radio = new PG_RadioButton(this,
                                                   PG_Rect(x,0,0,0),
                                                   "Disable",
                                                   audioscroll_radio);
  this->m_disablescroll_radio->SetSizeByText();
  this->m_disablescroll_radio->sigClick.connect(slot(*this, &WidgetRecognitionArea::handle_radio), (void*)DISABLE);
  
  // Set autoscrolling to follow recognizer by default.
  recognizerscroll_radio->SetPressed();
  this->m_autoscroll = RECOGNIZER;

  // Create time axis.
  this->m_time_axis = new WidgetTimeAxis(this,
                                         PG_Rect(0, 30, this->Width(), 30),
                                         pixels_per_second);

  // Wave and spectrogram views have relative heights whereas other components
  // have absolute heights. Thus making this widget higher makes wave and
  // spectrogram higher.
  const int top = 65;
  const int bottom = this->my_height - 55;
  const int height = bottom - top;
  const int wave_height = (int)(0.4 * height);
  const int spectrogram_height = (int)(0.6 * height);
  assert(height > 0);
  
  // Create wave view.
  this->m_wave = new WidgetWave(this,
                                PG_Rect(0, top, this->Width(), wave_height),
                                audio_input,
                                pixels_per_second);
  this->m_wave->initialize();
  
  // Create spectrogram.
  // ADJUST: The last two parameters affect the magnitudes. You can adjust them
  // to get better visualization.
  this->m_spectrogram = new WidgetSpectrogram(this,
                                              PG_Rect(0,
                                                      top + wave_height,
                                                      this->Width(),
                                                      spectrogram_height),
                                              audio_input,
                                              pixels_per_second,
                                              0.35,
                                              0.9985);
  this->m_spectrogram->initialize();
  // ADJUST: Map the y axis! You may adjust these values to modify the y axis.
  // It is part logarithmic part linear axis.
  this->m_spectrogram->create_y_axis(0.63, 0.21); 
                                                
  
  // Create recognition text area.
  this->m_text_area = new WidgetRecognitionText(this,
                                                 PG_Rect(0,
                                                         this->my_height - 50,
                                                         this->Width(),
                                                         30),
                                                 audio_input,
                                                 recognition,
                                                 pixels_per_second);
  
  // Create scroll bar.
  this->m_scroll_bar = new WidgetScrollBar(this,
                                           PG_Rect(0,
                                                   this->Height() - 15,
                                                   this->Width(),
                                                   10),
                                           PG_ScrollBar::HORIZONTAL);
  this->m_scroll_bar->SetPageSize(this->Width());
  this->m_scroll_bar->SetLineSize(30);
  this->set_scroll_range();
  this->m_scroll_bar->sigScrollPos.connect(slot(*this, &WidgetRecognitionArea::handle_scroll));
  this->m_scroll_bar->sigScrollTrack.connect(slot(*this, &WidgetRecognitionArea::handle_scroll));
}

WidgetRecognitionArea::~WidgetRecognitionArea()
{
  this->m_wave->terminate();
  this->m_spectrogram->terminate();
}

void
WidgetRecognitionArea::reset()
{
  // Reset components.
  this->m_wave->reset();
  this->m_spectrogram->reset();
  this->m_text_area->reset();
  this->m_time_axis->reset();
  // Update scroll.
  this->set_scroll_range();
  this->set_scroll_position(this->m_scroll_bar->GetPosition());
}

void
WidgetRecognitionArea::set_scroll_position(unsigned long page)
{
  unsigned long max = this->m_scroll_bar->GetMaxRange();
  page = page <= max ? page : max;

  // Set scroll position for each component.
  this->m_scroll_bar->SetPosition(page);
  this->m_text_area->set_scroll_position(page);
  this->m_time_axis->set_scroll_position(page);
  this->m_wave->set_scroll_position(page);
  this->m_spectrogram->set_scroll_position(page);
}

void
WidgetRecognitionArea::update_cursors()
{
  // Draw the line cursors.
  this->draw_cursor(this->get_audio_cursor(), PG_Color(255, 255, 255));
  this->draw_cursor(this->get_recognizer_cursor(), PG_Color(255, 0, 255));
  if (this->m_audio_input->is_playbacking())
    this->draw_cursor(this->get_playback_cursor(), PG_Color(255, 255, 0));
}

void
WidgetRecognitionArea::draw_cursor(long position, PG_Color color)
{
  // Draw a vertical line to given data position.
  if (position >= this->m_scroll_bar->GetPosition() &&
      position <= this->m_scroll_bar->GetPosition() + this->Width()) {
        
    this->m_wave->DrawVLine(position - this->m_scroll_bar->GetPosition(),
                            0,
                            this->m_wave->Height(),
                            color);

    this->m_spectrogram->DrawVLine(position - this->m_scroll_bar->GetPosition(),
                                   0,
                                   this->m_spectrogram->Height(),
                                   color);
  }

}

void
WidgetRecognitionArea::update_screen(bool new_data)
{
  this->m_wave->update();
  this->m_spectrogram->update();
  if (new_data)
    this->m_text_area->update();
  this->m_time_axis->update(this->m_audio_input->get_audio_data_size() / audio::audio_sample_rate);
    
  this->update_cursors();

  // Update the screen.
  this->Update(true);
}

void
WidgetRecognitionArea::update()
{
  this->set_scroll_range();

  // Autoscroll.
  if (this->m_autoscroll != DISABLE) {
    long read_cursor_px = 0;
    unsigned long scroll_pos = 0;
    bool scrolling = true;
    if (this->m_autoscroll == AUDIO)
      read_cursor_px = this->get_audio_cursor();
    else if (this->m_autoscroll == RECOGNIZER)
      read_cursor_px = this->get_recognizer_cursor();
    else
      scrolling = false;
    if (scrolling) {
      if (read_cursor_px < this->m_scroll_bar->GetPosition() ||
          read_cursor_px > this->m_scroll_bar->GetPosition() + this->Width()) {

        if (read_cursor_px > this->Width() / 2) {
          scroll_pos = read_cursor_px - this->Width() / 2;
        }
        this->set_scroll_position(scroll_pos);
      }
    }
  }
  else {
    // This will do redrawing for scroll bar. (Update and Redraw won't work properly..)
    this->m_scroll_bar->SetPosition(this->m_scroll_bar->GetPosition());
  }
  
  // Update screen.
  this->update_screen(true);
}

void
WidgetRecognitionArea::set_scroll_range()
{
  unsigned long audio_data_pixels = this->get_audio_pixels();
  // Set the range of the scroll bar.
  if (audio_data_pixels > this->Width())
    this->m_scroll_bar->SetRange(0, audio_data_pixels - this->Width());
  else
    this->m_scroll_bar->SetRange(0, 0);
}

bool
WidgetRecognitionArea::handle_scroll(PG_ScrollBar *scroll_bar, long page)
{
  // When user scrolls, disable autoscroll.
  this->m_autoscroll = DISABLE;
  this->m_disablescroll_radio->SetPressed();
  this->set_scroll_position(page);
  this->update_screen(false);
  return true;
}

bool
WidgetRecognitionArea::handle_radio(PG_RadioButton *radio, bool status, void *user_data)
{
  if ((int)user_data == AUDIO) {
    this->m_autoscroll = AUDIO;
  }
  if ((int)user_data == RECOGNIZER) {
    this->m_autoscroll = RECOGNIZER;
  }
  if ((int)user_data == DISABLE) {
    this->m_autoscroll = DISABLE;
  }
  return true;
}

