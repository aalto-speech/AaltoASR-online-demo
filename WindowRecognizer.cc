
#include "Recognition.hh"
#include "WindowMessageBox.hh"
#include "WindowRecognizer.hh"
#include "WindowReset.hh"
#include "WindowSettings.hh"
#include "WindowStartProcess.hh"
#include "str.hh"

WindowRecognizer::WindowRecognizer(Process *process, msg::InQueue *in_queue, msg::OutQueue *out_queue)
  : m_queue(in_queue, &m_recognition)
{
  
  this->m_back_button = NULL;
  this->m_play_button = NULL;
  this->m_reset_button = NULL;
  this->m_resetrecog_button = NULL;
  this->m_enablerecog_button = NULL;
  this->m_endaudio_button = NULL;
  this->m_settings_button = NULL;

  this->m_recognition_area = NULL;
  
  this->m_process = process;
  this->m_in_queue = in_queue;
  this->m_out_queue = out_queue;
  
  this->m_paused = false;
}

WindowRecognizer::~WindowRecognizer()
{
  delete this->m_back_button;
  delete this->m_play_button;
  delete this->m_reset_button;
  delete this->m_resetrecog_button;
  delete this->m_enablerecog_button;
  delete this->m_endaudio_button;
  delete this->m_settings_button;
  
  delete this->m_recognition_area;
}

void
WindowRecognizer::initialize()
{
  Window::initialize();
  
  this->m_play_button = new PG_Button(this->m_window, PG_Rect(200,100,150,50), "Play");
  this->m_enablerecog_button = new PG_Button(this->m_window, PG_Rect(200,200,150,50), "Enable recognizer");
  this->m_endaudio_button = new PG_Button(this->m_window, PG_Rect(200,300,150,50), "End audio");
  this->m_reset_button = new PG_Button(this->m_window, PG_Rect(400,100,150,50), "Reset");
  this->m_resetrecog_button = new PG_Button(this->m_window, PG_Rect(400,200,150,50), "Reset recognizer");
  this->m_settings_button = new PG_Button(this->m_window, PG_Rect(400,300,150,50), "Settings..");
  this->m_back_button = new PG_Button(this->m_window, PG_Rect(this->m_window->my_width - 200,this->m_window->my_height-80,150,50), "Back to Menu", PG_Button::CANCEL);

  this->m_window->AddChild(this->m_play_button);
  this->m_window->AddChild(this->m_enablerecog_button);
  this->m_window->AddChild(this->m_endaudio_button);
  this->m_window->AddChild(this->m_reset_button);
  this->m_window->AddChild(this->m_resetrecog_button);
  this->m_window->AddChild(this->m_settings_button);
  this->m_window->AddChild(this->m_back_button);

  this->m_play_button->sigClick.connect(slot(*this, &WindowRecognizer::handle_play_button));
  this->m_enablerecog_button->sigClick.connect(slot(*this, &WindowRecognizer::handle_enablerecog_button));
  this->m_endaudio_button->sigClick.connect(slot(*this, &WindowRecognizer::handle_endaudio_button));
  this->m_reset_button->sigClick.connect(slot(*this, &WindowRecognizer::handle_reset_button));
  this->m_resetrecog_button->sigClick.connect(slot(*this, &WindowRecognizer::handle_resetrecog_button));
  this->m_settings_button->sigClick.connect(slot(*this, &WindowRecognizer::handle_settings_button));
  this->m_back_button->sigClick.connect(slot(*this, &WindowRecognizer::handle_back_button));
  
  this->m_play_button->SetToggle(true);
  this->m_enablerecog_button->SetToggle(true);
}

void
WindowRecognizer::do_opening()
{
  this->pause_audio_input(true);
  
  this->m_recognition_area = new WidgetRecognitionArea(this->m_window,
                                                       PG_Rect(0, 400, this->m_window->Width(), 200),
                                                       this->get_audio_input(),
                                                       &this->m_recognition,
                                                       250);
  this->m_window->AddChild(this->m_recognition_area);
  this->m_recognition_area->Show(false);

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
  bool restart = true;
  
  this->pause_window_functionality(true);

  this->error("Recognition process disconnected.\nPress OK to restart.", ERROR_NORMAL);

  WindowStartProcess window(this->m_window, this->m_process, this->m_in_queue, this->m_out_queue);
  window.initialize();
  ret_val = this->run_child_window(&window);
  if (ret_val != 0) {
    this->reset(false);
    this->pause_window_functionality(false);
  }
  /*
  while (restart) {
    try {
      ret_val = this->run_child_window(&window);
      this->reset(false);
      this->pause_window_functionality(false);
      restart = false;
    }
    catch (msg::ExceptionBrokenPipe exception) {
      WindowMessageBox message(this->m_window,
                               "Error",
                               "Recognizer process couldn't be restarted.\nDo you want to retry or exit?",
                               "Retry",
                               "Exit");
      message.initialize();
      ret_val = this->run_child_window(&message);
      restart = (ret_val == 1);
      if (ret_val == 2)
        this->end_running(0);
    }
  }
  //*/
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
    this->pause_audio_input(true);
    this->m_queue.disable();
  }
  else {
    this->m_queue.enable();
  }
}

void
WindowRecognizer::reset(bool reset_audio)
{
  WindowReset window(this->m_window, this->m_in_queue, this->m_out_queue);

  this->pause_window_functionality(true);

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
    
  if (this->m_recognition_area)
    this->m_recognition_area->set_autoscroll(!pause);
      
  this->m_paused = pause;
}

void
WindowRecognizer::end_of_audio()
{
  this->pause_audio_input(true);
  this->set_status(END_OF_AUDIO);
  if (this->m_recognition_area)
    this->m_recognition_area->set_autoscroll(false);
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
//    this->m_play_button->Hide(false);
//    this->m_endaudio_button->Hide(false);
    break;
  case LISTENING:
//    this->m_play_button->Show(false);
//    this->m_endaudio_button->Show(false);
    break;
  default:
    fprintf(stderr, "WindowRecognizer::set_status unknown status.\n");
  }
}

bool
WindowRecognizer::handle_play_button(PG_Button *button)
{
  if (this->m_status == LISTENING) {
    this->pause_audio_input(!this->m_play_button->GetPressed());
  }
  return true;
}

bool
WindowRecognizer::handle_enablerecog_button(PG_Button *button)
{
  if (this->m_enablerecog_button->GetPressed())
    this->m_queue.enable();
  else
    this->m_queue.disable();
  /*
  if (this->m_enablerecog_button->GetPressed())
    this->get_audio_input()->play(10 * SAMPLE_RATE, 0);
  else
    this->get_audio_input()->stop();
  //*/
  return true;
}

bool
WindowRecognizer::handle_endaudio_button(PG_Button *button)
{
  if (this->m_status == LISTENING) {
    this->end_of_audio();
  }
  return true;
}

bool
WindowRecognizer::handle_reset_button(PG_Button *button)
{
  this->reset(true);
  return true;
}

bool
WindowRecognizer::handle_resetrecog_button(PG_Button *button)
{
  this->reset(false);
  return true;
}

bool
WindowRecognizer::handle_settings_button(PG_Button *button)
{
  this->pause_window_functionality(true);
  WindowSettings window(this->m_window, this->m_out_queue);
  window.initialize();
  this->run_child_window(&window);
  this->pause_window_functionality(false);
  return true;
}
bool
WindowRecognizer::handle_back_button(PG_Button *button)
{
  this->end_running(1);
  return true;
}
