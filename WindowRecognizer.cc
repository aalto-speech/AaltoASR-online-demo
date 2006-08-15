
#include <pgapplication.h>
#include "WindowRecognizer.hh"
#include "WindowMessageBox.hh"
#include "WindowReset.hh"
#include "WindowSettings.hh"
#include "WindowStartProcess.hh"
#include "str.hh"

//WindowRecognizer::WindowRecognizer(Process *process, msg::InQueue *in_queue, msg::OutQueue *out_queue)
WindowRecognizer::WindowRecognizer(RecognizerProcess *recognizer)
  : m_queue(recognizer ? recognizer->get_in_queue() : NULL,
            &m_recognition)
{
  
  this->m_play_button = NULL;
  this->m_endaudio_button = NULL;
  this->m_enablerecog_button = NULL;
  this->m_adapt_button = NULL;

  this->m_recognition_area = NULL;
  this->m_texts_area = NULL;
  
  this->m_recognizer = recognizer;
//  this->m_process = process;
//  this->m_in_queue = in_queue;
//  this->m_out_queue = out_queue;
  
  this->m_broken_pipe = false;
  
}

void
WindowRecognizer::initialize()
{
  Window::initialize();
  
  this->m_play_button =
    this->construct_button("RUN", 1, 0, slot(*this, &WindowRecognizer::handle_play_button));
  this->m_endaudio_button =
    this->construct_button("End audio", 1, 1, slot(*this, &WindowRecognizer::handle_endaudio_button));
  this->m_enablerecog_button =
    this->construct_button("Enable recognizer", 1, 2, slot(*this, &WindowRecognizer::handle_enablerecog_button));
  this->construct_button("Reset", 2, 1, slot(*this, &WindowRecognizer::handle_reset_button));
  this->construct_button("Reset recognizer", 2, 0, slot(*this, &WindowRecognizer::handle_resetrecog_button));
  this->construct_button("Settings..", 2, 2, slot(*this, &WindowRecognizer::handle_settings_button));
  this->construct_button("Close", 0, 0, slot(*this, &WindowRecognizer::handle_back_button));
  this->m_adapt_button =
    this->construct_button("Adapt speaker", 3, 0, slot(*this, &WindowRecognizer::handle_adapt_button));
  this->construct_button("Reset adaptation", 3, 1, slot(*this, &WindowRecognizer::handle_resetadaptation_button));

  this->m_play_button->SetToggle(true);
  this->m_enablerecog_button->SetToggle(true);
  this->m_adapt_button->SetToggle(true);
  
}

void
WindowRecognizer::do_opening()
{
  // These constants are used to construct the following areas.
  const unsigned int top = 180;
  const unsigned int space = 10;
  const unsigned int bottom = 10;
  const unsigned int height = this->m_window->Height() - (top + space + bottom);
  const float text_part = 0.4;
  const float recognizer_part = 1.0 - text_part;

  // Create area for original and recognized texts.
  this->m_texts_area = new WidgetComparisonArea(*this,
                                           PG_Rect(0,
                                                   top,
                                                   this->m_window->Width(),
                                                   (int)(text_part * height)),
                                           &this->m_recognition);
                                           
  // Create area for wave, spectrogram and recognition text.
  this->m_recognition_area =
    new WidgetRecognitionArea(this->m_window,
                              PG_Rect(0,
                                      (int)(top + space + text_part * height),
                                      this->m_window->Width(),
                                      (int)(recognizer_part * height)),
                              this->get_audio_input(),
                              &this->m_recognition,
                              250);

  // Enable recognizer.
  this->m_enablerecog_button->SetPressed(true);
  this->m_queue.enable();
  if (!this->m_queue.start()) {
    this->error("Couldn't start queue in WindowRecognizer::open.", ERROR_CLOSE);
    return;
  }

  // Do reseting
  this->reset(true);
  this->set_status(LISTENING);
  this->pause_audio_input(true);
  
//  PG_Application::SetBulkMode(true);
}

void
WindowRecognizer::do_running()
{
  /*
  static unsigned long luku = 0;
  luku++;
  if (luku % 1000 == 0)
    fprintf(stderr, "%d\n", luku);
  //*/

  // Check broken pipe flags.
  if (this->m_broken_pipe || this->m_queue.is_broken_pipe()) {
    this->m_queue.stop();
    this->m_queue.disable();
    this->handle_broken_pipe();
    this->m_queue.start();
  }
  else {
    unsigned long read_size = this->get_audio_input()->operate();
//    if (read_size > 0) {
      this->flush_out_queue();
//      fprintf(stderr, "read_size: %d\n", read_size);
//    }
    this->m_recognition_area->update();
  }
  
}

void
WindowRecognizer::do_closing(int return_value)
{
//  PG_Application::SetBulkMode(false);

  this->m_queue.stop();
  
  if (this->m_texts_area) {
    this->m_window->RemoveChild(this->m_texts_area);
    delete this->m_texts_area;
    this->m_texts_area = NULL;
  }    

  if (this->m_recognition_area) {
    this->m_window->RemoveChild(this->m_recognition_area);
    delete this->m_recognition_area;
    this->m_recognition_area = NULL;
  }

}

bool
WindowRecognizer::handle_broken_pipe()
{
  int ret_val;

  this->m_broken_pipe = true;  
  if (!this->error("Recognition process disconnected.\nPress OK to restart.",
                   ERROR_NORMAL)) {
    return false;
  }

  WindowStartProcess window(this->m_window, this->m_recognizer);//this->m_process, this->m_in_queue, this->m_out_queue);
  window.initialize();
  
  // NOTE: We have to close audio stream or the forked process will have its own
  // copy of this stream open and in that case we couldn't open audio streams
  // anymore. (Don't know if there's a better way to deal with this? fcloseall
  // did not work..)
  this->get_audio_input()->terminate();

  ret_val = this->run_child_window(&window);

  // Check that whole application wasn't terminated.  
  if (ret_val != 0) {
    this->reset(false);
    this->handle_adapt_button();
    this->m_broken_pipe = false;

    // Now re-open the audio stream (look at the note above).
    if (!this->get_audio_input()->initialize()) {
      this->error("Failed to re-initialize audio input.", ERROR_CLOSE);
      ret_val = 0;
    }
  }

  return !this->m_broken_pipe;
}
//*
void
WindowRecognizer::flush_out_queue()
{
  try {
    if (this->m_recognizer)
      this->m_recognizer->get_out_queue()->flush();
  }
  catch (msg::ExceptionBrokenPipe exception) {
    this->m_broken_pipe = true;
//    this->handle_broken_pipe();
  }
}
//*/
void
WindowRecognizer::pause_window_functionality(bool pause)
{
  if (pause) {
    Window::pause_window_functionality(true);
    this->get_audio_input()->pause_listening(true);
    this->enable_recognizer(false);
    this->m_queue.disable();
  }
  else {
    this->m_queue.enable();
    this->enable_recognizer(true);
    this->get_audio_input()->pause_listening(!this->m_play_button->GetPressed());
    Window::pause_window_functionality(false);
  }
}

void
WindowRecognizer::reset(bool reset_audio)
{
  WindowReset window(this->m_window,
                     this->m_recognizer ? this->m_recognizer->get_in_queue() : NULL,
                     this->m_recognizer ? this->m_recognizer->get_out_queue() : NULL);

  // Window functionality will be paused and unpaused when running a child
  // window but we want to do these pauses before running the child window
  // because we are resetting some shared resources.
  this->pause_window_functionality(true);
  this->pause_audio_input(true);

  if (reset_audio)
    this->get_audio_input()->reset();
  else
    this->get_audio_input()->reset_cursors();

  // Reset recognition before recognition area, because it resets and updates
  // according to recognition.
  this->m_recognition.reset();
  this->m_recognition_area->reset();
  this->m_recognition_area->update();

  window.initialize();
  if (this->run_child_window(&window) == -1) {
    this->m_broken_pipe = true;
    return;
  }

  this->handle_enablerecog_button();
}

void
WindowRecognizer::pause_audio_input(bool pause)
{
  this->get_audio_input()->pause_listening(pause);
  this->m_play_button->SetPressed(!pause);
}

void
WindowRecognizer::end_of_audio()
{
  this->pause_audio_input(true);
  this->set_status(END_OF_AUDIO);
  if (this->m_recognizer) {
    msg::Message message(msg::M_AUDIO_END, true);
    this->m_recognizer->get_out_queue()->add_message(message);
//    this->m_out_queue->add_message(msg::Message(msg::M_AUDIO_END));
    this->flush_out_queue();
  }
}

void
WindowRecognizer::set_status(Status status)
{
  this->m_status = status;
  switch (status) {
  case END_OF_AUDIO:
    this->m_play_button->Hide(false);
    this->m_endaudio_button->Hide(false);
    break;
  case LISTENING:
    this->m_play_button->Show(false);
    this->m_endaudio_button->Show(false);
    break;
  default:
    fprintf(stderr, "WindowRecognizer::set_status unknown status.\n");
  }
}

void
WindowRecognizer::enable_recognizer(bool enable)
{
  if (this->m_recognizer) {

    msg::Message message(msg::M_DECODER_PAUSE, true);
    if (enable)
      message.set_type(msg::M_DECODER_UNPAUSE);

    this->m_recognizer->get_out_queue()->add_message(message);
    this->flush_out_queue();
  }
}

bool
WindowRecognizer::handle_play_button()
{
  if (this->m_status == LISTENING) {
    this->pause_audio_input(!this->m_play_button->GetPressed());
  }
  return true;
}

bool
WindowRecognizer::handle_enablerecog_button()
{
  this->enable_recognizer(this->m_enablerecog_button->GetPressed());
  return true;
}

bool
WindowRecognizer::handle_endaudio_button()
{
  if (this->m_status == LISTENING) {
    this->end_of_audio();
  }
  return true;
}

bool
WindowRecognizer::handle_reset_button()
{
  this->reset(true);
  return true;
}

bool
WindowRecognizer::handle_resetrecog_button()
{
  this->reset(false);
  return true;
}

bool
WindowRecognizer::handle_settings_button()
{
  WindowSettings window(this->m_window, this->m_recognizer);
  window.initialize();
  if (this->run_child_window(&window) == -1) {
    this->m_broken_pipe = true;
  }
  return true;
}

bool
WindowRecognizer::handle_back_button()
{
  this->end_running(1);
  return true;
}

bool
WindowRecognizer::handle_adapt_button()
{
  msg::Message message(msg::M_ADAPT_ON);//, true);
  
  if (this->m_adapt_button->GetPressed()) {
    message.set_type(msg::M_ADAPT_ON);
//    this->reset(false);
  }
  else {
    message.set_type(msg::M_ADAPT_OFF);
//    if (this->m_status != END_OF_AUDIO)
//      this->end_of_audio();
  }

  if (this->m_recognizer) {
    this->m_recognizer->get_out_queue()->add_message(message);
    this->flush_out_queue();
  }
  
  return true;
}

bool
WindowRecognizer::handle_resetadaptation_button()
{
//  this->error("Reset adaptation not implemented.", ERROR_NORMAL);
  if (this->m_recognizer) {
    msg::Message message(msg::M_ADAPT_RESET, true);
    this->m_recognizer->get_out_queue()->add_message(message);
    this->flush_out_queue();
  }
  
  return true;
}

PG_Button*
WindowRecognizer::construct_button(const std::string &label,
                                   unsigned int column_index,
                                   unsigned int row_index,
                                   const SigC::Slot0<bool> &callback)
{
  const int width = 160;
  const int height = 50;
  const int horizontal_space = 50;
  const int vertical_space = 0;
  const int columns = 4;
  const int left_margin = (this->m_window->Width() - columns * width - (columns - 1) * horizontal_space) / 2;
  const int top_margin = 10;
  
  // Calculate rect.
  PG_Rect rect(left_margin + column_index * (horizontal_space + width),
               top_margin + row_index * (vertical_space + height),
               width,
               height);

  // Create button and connect it with the window.               
  PG_Button *button = new PG_Button(this->m_window, rect, label.data());
  button->sigClick.connect(callback);

  return button;
}
