
#include "AudioFileInputController.hh"

AudioFileInputController::AudioFileInputController(msg::OutQueue *out_queue)
  : AudioInputController(out_queue), m_output_buffer(48000)
{
  this->m_output_cursor = 0;
}

AudioFileInputController::~AudioFileInputController()
{
}

unsigned long
AudioFileInputController::get_audio_cursor() const
{
  return this->m_output_buffer.get_frames_read();
}

bool
AudioFileInputController::load_file(const std::string &filename)
{
  bool ret_val = false;
  if (audio::read_wav_data(filename, this->m_audio_data)) {
    this->reset_cursors();
    ret_val = true;
  }
  else {
    fprintf(stderr, "AudioFileInputController::load_file failed.\n");
    ret_val = false;
  }
  return ret_val;
}

void
AudioFileInputController::reset_cursors()
{
  this->m_output_buffer.clear();
  this->m_output_cursor = 0;
  AudioInputController::reset_cursors();
}

//*
void
AudioFileInputController::pause_listening(bool pause)
{
  AudioInputController::pause_listening(pause);
  if (!pause) {
//    this->m_audio_stream.set_output_buffer(&this->m_output_buffer);
    this->m_audio_stream.set_output_buffer(&this->m_output_buffer);
  }
}
//*/

void
AudioFileInputController::read_input()
{
  // Send audio to output stream.
  this->m_output_cursor += this->m_output_buffer.write(this->get_audio_data() + this->m_output_cursor,
                                                       this->get_audio_data_size() - this->m_output_cursor);

  // Return the frames that have been played (in speakers) since last read.
  // This way we will synchronize recognizer with the sound from speakers.
//  return this->m_output_buffer.get_frames_read() - this->get_read_cursor();
}
