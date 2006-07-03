
#include "WindowMicrophoneRecognizer.hh"
#include "WindowReset.hh"

WindowMicrophoneRecognizer::WindowMicrophoneRecognizer(msg::InQueue *in_queue, OutQueueController *out_queue)
  : WindowRecognizer(in_queue, out_queue)
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
  // ...  
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
}

void
WindowMicrophoneRecognizer::do_closing()
{
  WindowRecognizer::do_closing();
  if (this->m_audio_input) {
    this->m_audio_input->terminate();
    delete this->m_audio_input;
    this->m_audio_input = NULL;
  }
}
/*
bool
WindowFileRecognizer::button_pressed(PG_Button *button)
{
  return true;
}
//*/
