
#include "WindowMicrophoneRecognizer.hh"
#include "WindowSaveAudioFile.hh"

WindowMicrophoneRecognizer::WindowMicrophoneRecognizer(Process *process,
                                                       msg::InQueue *in_queue,
                                                       msg::OutQueue *out_queue)
  : WindowRecognizer(process, in_queue, out_queue)
{
  this->m_audio_input = NULL;
}

WindowMicrophoneRecognizer::~WindowMicrophoneRecognizer()
{
  delete this->m_audio_input;
}

void
WindowMicrophoneRecognizer::initialize()
{
  WindowRecognizer::initialize();

  this->construct_button("Save audio", 0, 2, slot(*this, &WindowMicrophoneRecognizer::handle_save_button));
}

void
WindowMicrophoneRecognizer::do_opening()
{
  this->m_audio_input = new AudioLineInputController(this->m_out_queue);
  if (!this->m_audio_input->initialize()) {
    this->error("Audio input controller initialization failed. Try closing all other programs.", ERROR_CLOSE);
    return;
  }
  WindowRecognizer::do_opening();
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

bool
WindowMicrophoneRecognizer::handle_save_button()
{
  this->pause_window_functionality(true);
  this->pause_audio_input(true);

  if (this->m_audio_input->get_audio_data_size()) {
    WindowSaveAudioFile window(this->m_window, this->m_audio_input);
    window.initialize();
    this->run_child_window(&window);
  }
  else {
    this->error("No audio to save. Record some audio first using Run-button.", ERROR_NORMAL);
  }

  this->pause_window_functionality(false);

  return true;
}
