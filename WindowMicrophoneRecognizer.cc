
#include "WindowMicrophoneRecognizer.hh"
#include "WindowSaveFile.hh"

WindowMicrophoneRecognizer::WindowMicrophoneRecognizer(msg::InQueue *in_queue, OutQueueController *out_queue)
  : WindowRecognizer(in_queue, out_queue)
{
  this->m_audio_input = NULL;
  this->m_save_button = NULL;
  this->m_save_pressed = false;
}

WindowMicrophoneRecognizer::~WindowMicrophoneRecognizer()
{
  delete this->m_audio_input;
}

void
WindowMicrophoneRecognizer::initialize()
{
  WindowRecognizer::initialize();
  // ...  
  this->m_save_button = new PG_Button(this->m_window, PG_Rect(10,200,150,50), "Save");
  this->m_save_button->sigClick.connect(slot(*this, &WindowMicrophoneRecognizer::handle_button));
  this->m_window->AddChild(this->m_save_button);
}

void
WindowMicrophoneRecognizer::do_opening()
{
  this->m_audio_input = new AudioLineInputController(this->m_out_queue);
  if (!this->m_audio_input->initialize()) {
    this->error("Audio input controller initialization failed.", ERROR_CLOSE);
    return;
  }
  WindowRecognizer::do_opening();
}

void
WindowMicrophoneRecognizer::do_running()
{
  WindowRecognizer::do_running();
  
  if (this->m_save_pressed) {
    this->pause_window_functionality(true);
    this->save_audio_file();
    this->m_save_pressed = false;
    this->pause_window_functionality(false);
  }
}

void
WindowMicrophoneRecognizer::do_closing(int return_value)
{
  WindowRecognizer::do_closing(return_value);
  if (this->m_audio_input) {
    this->m_audio_input->terminate();
    delete this->m_audio_input;
    this->m_audio_input = NULL;
  }
}

void
WindowMicrophoneRecognizer::reset(bool reset_audio)
{
  WindowRecognizer::reset(reset_audio);
  this->set_status(LISTENING);
} 

void
WindowMicrophoneRecognizer::save_audio_file()
{
  if (this->m_audio_input->get_audio_data_size()) {
    WindowSaveFile window(this->m_window, this->m_audio_input);
    window.initialize();
    this->run_child_window(&window);
  }
  else {
    this->error("No audio to save. Record some audio first.", ERROR_NORMAL);
  }
}

bool
WindowMicrophoneRecognizer::handle_button(PG_Button *button)
{
  if (button == this->m_save_button) {
    this->m_save_pressed = true;
  }
  return true;
}
