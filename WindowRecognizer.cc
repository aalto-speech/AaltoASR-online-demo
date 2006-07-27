
#include "Recognition.hh"
#include "WindowMessageBox.hh"
#include "WindowRecognizer.hh"
#include "WindowReset.hh"
#include "WindowSettings.hh"
#include "WindowStartProcess.hh"
#include "WindowText.hh"
#include "str.hh"

WindowRecognizer::WindowRecognizer(Process *process, msg::InQueue *in_queue, msg::OutQueue *out_queue)
  : m_queue(in_queue, &m_recognition)
{
  
  this->m_play_button = NULL;
  this->m_endaudio_button = NULL;
  this->m_enablerecog_button = NULL;

  this->m_recognition_area = NULL;
  
  this->m_process = process;
  this->m_in_queue = in_queue;
  this->m_out_queue = out_queue;
  
}

void
WindowRecognizer::initialize()
{
  Window::initialize();
  
  this->m_play_button =
    this->construct_button("Run", 1, 0, slot(*this, &WindowRecognizer::handle_play_button));
  this->m_endaudio_button =
    this->construct_button("End audio", 1, 1, slot(*this, &WindowRecognizer::handle_endaudio_button));
  this->m_enablerecog_button =
    this->construct_button("Enable recognizer", 1, 2, slot(*this, &WindowRecognizer::handle_enablerecog_button));
  this->construct_button("Reset", 2, 1, slot(*this, &WindowRecognizer::handle_reset_button));
  this->construct_button("Reset recognizer", 2, 0, slot(*this, &WindowRecognizer::handle_resetrecog_button));
  this->construct_button("Settings..", 2, 2, slot(*this, &WindowRecognizer::handle_settings_button));
  this->construct_button("Close", 0, 0, slot(*this, &WindowRecognizer::handle_back_button));
  this->construct_button("Show text", 3, 0, slot(*this, &WindowRecognizer::handle_showtext_button));

  this->m_play_button->SetToggle(true);
  this->m_enablerecog_button->SetToggle(true);
}

void
WindowRecognizer::do_opening()
{
  // Create recognition area.
  this->m_recognition_area =
    new WidgetRecognitionArea(this->m_window,
                              PG_Rect(0, 400, this->m_window->Width(), 400),
                              this->get_audio_input(),
                              &this->m_recognition,
                              250);
//  this->m_recognition_area->Show(false);

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

  // Check broken pipe flag from queue controller.
  if (this->m_queue.is_broken_pipe()) {
    this->m_queue.stop();
    this->m_queue.disable();
    this->m_queue.start();
    this->handle_broken_pipe();
  }
  else {
    unsigned long read_size = this->get_audio_input()->operate();
    if (read_size > 0) {
      this->flush_out_queue();
//      fprintf(stderr, "read_size: %d\n", read_size);
    }
    this->m_recognition_area->update();
  }
}

void
WindowRecognizer::do_closing(int return_value)
{
  this->m_queue.stop();

  if (this->m_recognition_area) {
    this->m_window->RemoveChild(this->m_recognition_area);
    delete this->m_recognition_area;
    this->m_recognition_area = NULL;
  }
}

void
WindowRecognizer::handle_broken_pipe()
{
  int ret_val;
  
  this->pause_window_functionality(true);

  this->error("Recognition process disconnected.\nPress OK to restart.", ERROR_NORMAL);

  WindowStartProcess window(this->m_window, this->m_process, this->m_in_queue, this->m_out_queue);
  window.initialize();
  ret_val = this->run_child_window(&window);
  if (ret_val != 0) {
    this->reset(false);
    this->pause_window_functionality(false);
  }
}
//*
void
WindowRecognizer::flush_out_queue()
{
  try {
    if (this->m_out_queue)
      this->m_out_queue->flush();
  }
  catch (msg::ExceptionBrokenPipe exception) {
    this->handle_broken_pipe();
  }
}
//*/
void
WindowRecognizer::pause_window_functionality(bool pause)
{
  if (pause) {
    this->get_audio_input()->pause_listening(true);
    this->enable_recognizer(false);
    this->m_queue.disable();
  }
  else {
    this->m_queue.enable();
    this->enable_recognizer(true);
    this->get_audio_input()->pause_listening(!this->m_play_button->GetPressed());
  }
}

void
WindowRecognizer::reset(bool reset_audio)
{
  WindowReset window(this->m_window, this->m_in_queue, this->m_out_queue);

  this->pause_window_functionality(true);
  this->pause_audio_input(true);

  if (reset_audio)
    this->get_audio_input()->reset();
  else
    this->get_audio_input()->reset_cursors();

  this->m_recognition.reset();
  this->m_recognition_area->reset();
  this->m_recognition_area->update();

  window.initialize();
  try {
    this->run_child_window(&window);
    this->handle_enablerecog_button();
  }
  catch (msg::ExceptionBrokenPipe exception) {
    this->handle_broken_pipe();
  }
  
  this->pause_window_functionality(false);
}

void
WindowRecognizer::pause_audio_input(bool pause)
{
  this->get_audio_input()->pause_listening(pause);
  this->m_play_button->SetPressed(!pause);
    
//  if (this->m_recognition_area)
//    this->m_recognition_area->set_autoscroll(!pause);
      
//  this->m_paused = pause;
}

void
WindowRecognizer::end_of_audio()
{
  this->pause_audio_input(true);
  this->set_status(END_OF_AUDIO);
//  if (this->m_recognition_area)
//    this->m_recognition_area->set_autoscroll(DISABLE);
  if (this->m_out_queue) {
    this->m_out_queue->add_message(msg::Message(msg::M_AUDIO_END));
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
  msg::Message message(msg::M_DECODER_PAUSE, true);

  if (enable) {
    fprintf(stderr, "Recognizer enabled\n");
    message.set_type(msg::M_DECODER_UNPAUSE);
  }
  else
    fprintf(stderr, "Recognizer disabled\n");

  if (this->m_out_queue)
    this->m_out_queue->add_message(message);

  this->flush_out_queue();
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
  this->pause_window_functionality(true);
  WindowSettings window(this->m_window, this->m_out_queue);
  window.initialize();
  try {
    this->run_child_window(&window);
  }
  catch (msg::ExceptionBrokenPipe) {
    this->handle_broken_pipe();
  }
  this->pause_window_functionality(false);
  return true;
}

bool
WindowRecognizer::handle_back_button()
{
  this->end_running(1);
  return true;
}

bool
WindowRecognizer::handle_showtext_button()
{
  this->pause_window_functionality(true); // This disables the other thread.
  // this->m_recognition.lock(); // No need, because other thread disabled.
  WindowText window(this->m_window,
                    "Recognition",
                    this->m_recognition.get_recognition_text());
  // this->m_recognition.unlock(); // No need, because other thread disabled.
  window.initialize();
  this->run_child_window(&window);
  this->pause_window_functionality(false);
  return true;
}

PG_Button*
WindowRecognizer::construct_button(const std::string &label,
                                   unsigned int column_index,
                                   unsigned int row_index,
                                   const SigC::Slot0<bool> &callback)
{
  const int left_margin = 10;
  const int top_margin = 10;
  const int width = 160;
  const int height = 50;
  const int horizontal_space = 50;
  const int vertical_space = 0;
  
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
