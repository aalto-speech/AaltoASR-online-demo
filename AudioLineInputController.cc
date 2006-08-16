
#include "AudioLineInputController.hh"

AudioLineInputController::AudioLineInputController(msg::OutQueue *out_queue)
  : AudioInputController(out_queue), m_input_buffer(160000)
{
}

AudioLineInputController::~AudioLineInputController()
{
}

void
AudioLineInputController::pause_listening(bool pause)
{
  AudioInputController::pause_listening(pause);
  if (pause)
    this->m_audio_stream.set_input_buffer(NULL);
  else
    this->m_audio_stream.set_input_buffer(&this->m_input_buffer);
}

unsigned long
AudioLineInputController::get_audio_cursor() const
{
  return this->get_audio_data_size();
}

void
AudioLineInputController::reset_cursors()
{
  this->m_input_buffer.clear();
  AudioInputController::reset_cursors();
}

void
AudioLineInputController::read_input()
{
  this->m_input_buffer.read(this->m_audio_data);
}
