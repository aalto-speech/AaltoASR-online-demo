
#include "AudioInputStream.hh"
#include "AudioFileInputController.hh"

AudioFileInputController::AudioFileInputController(const char *filename, msg::OutQueue *out_queue)
  : AudioInputController(out_queue), m_audio_output(48000)
{
  audio::read_wav_data(filename, &this->m_audio_data);
  this->m_write_cursor = 0;
}

AudioFileInputController::~AudioFileInputController()
{
}

bool
AudioFileInputController::initialize()
{
  if (!AudioInputController::initialize())
    return false;
    
  if (!this->m_audio_output.open()) {
    fprintf(stderr, "WFIC initialization failed: Couldn't initialize audio output.\n");
    return false;
  }
  return true;
}

void
AudioFileInputController::terminate()
{
  this->m_audio_output.close();
  AudioInputController::terminate();
}

bool
AudioFileInputController::start_listening()
{
  if (!this->m_audio_output.start()) {
    fprintf(stderr, "WFIC.start_listening failed to start audio output.\n");
    return false;
  }

  // TODO: Tämä ennen tuota startia???
  this->m_write_cursor = 0;
  return AudioInputController::start_listening();
}
//*
void
AudioFileInputController::pause_listening(bool pause)
{
  this->m_audio_output.pause_output(pause);
  AudioInputController::pause_listening(pause);
}
//*/
unsigned long
AudioFileInputController::read_input()
{
  this->m_write_cursor += sizeof(AUDIO_FORMAT) *
                          this->m_audio_output.write_output(this->m_audio_data.data() + this->m_write_cursor,
                                                            (this->m_audio_data.length() - this->m_write_cursor) / sizeof(AUDIO_FORMAT));

  return this->m_audio_output.get_frames_played() - this->m_read_cursor / sizeof(AUDIO_FORMAT);
}
