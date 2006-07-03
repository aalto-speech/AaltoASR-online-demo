
#include "AudioInputStream.hh"
#include "AudioFileInputController.hh"

AudioFileInputController::AudioFileInputController(OutQueueController *out_queue)
  : AudioInputController(out_queue), m_audio_output(48000)
{
  this->m_write_cursor = 0;
}

AudioFileInputController::~AudioFileInputController()
{
}

bool
AudioFileInputController::load_file(const char *filename)
{
  bool ret_val = false;
  if (this->lock_audio_writing()) {
    this->reset();
    if (audio::read_wav_data(filename, &this->m_audio_data)) {
      ret_val = true;
    }
    else {
      fprintf(stderr, "AudioFileInputController::load_file failed.\n");
      this->reset();
      ret_val = false;
    }
    this->unlock_audio_writing();
  }
  return ret_val;
}

void
AudioFileInputController::reset()
{
  if (this->lock_audio_writing()) {
    this->m_audio_output.reset();
    this->m_write_cursor = 0;
    AudioInputController::reset();
    this->unlock_audio_writing();
  }
  else {
    fprintf(stderr, "AudioFileInputController::reset failed locking.\n");
  }
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
  if (!this->m_audio_output.start()) {
    fprintf(stderr, "WFIC.start_listening failed to start audio output.\n");
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
unsigned long AudioStream::luku = 0;


unsigned long
AudioFileInputController::read_input()
{
  if (this->lock_audio_writing()) {
    this->m_write_cursor += this->m_audio_output.write_output((AUDIO_FORMAT*)this->m_audio_data.data() + this->m_write_cursor,
                                                              this->m_audio_data.length() / sizeof(AUDIO_FORMAT) - this->m_write_cursor);
    
    this->unlock_audio_writing();

  }
//  fprintf(stderr, "Frame count: %d\n", this->m_audio_output.luku);
  // Return the frames that have been played (in speakers) since last read.
  // This way we will synchronize recognizer with the sound from speakers.
  return this->m_audio_output.get_frames_played() - this->get_read_cursor();
}
