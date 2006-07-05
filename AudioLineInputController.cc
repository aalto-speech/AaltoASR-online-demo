
#include "AudioLineInputController.hh"

AudioLineInputController::AudioLineInputController(OutQueueController *out_queue)
  : AudioInputController(out_queue), m_audio_input(160000)
{
//  this->m_read_cursor = 0;
}

AudioLineInputController::~AudioLineInputController()
{
}

bool
AudioLineInputController::initialize()
{
  if (!AudioInputController::initialize())
    return false;
    
  if (!this->m_audio_input.open()) {
    fprintf(stderr, "ALIC initialization failed: Couldn't initialize audio input.\n");
    return false;
  }
  if (!this->m_audio_input.start()) {
    fprintf(stderr, "ALIC.start_listening failed to start audio input.\n");
    return false;
  }

  return true;
}

void
AudioLineInputController::terminate()
{
  this->m_audio_input.close();
  AudioInputController::terminate();
}
/*
bool
AudioLineInputController::start_listening()
{
  return AudioInputController::start_listening();
}

void
AudioLineInputController::stop_listening()
{
  this->m_audio_input.pause_input(true);
  AudioInputController::stop_listening();
}
//*/
void
AudioLineInputController::pause_listening(bool pause)
{
  this->m_audio_input.pause_input(pause);
//  AudioInputController::pause_listening(pause);
//  fprintf(stderr, "ALIC: audio input paused: %d\n", (int)pause);
}

void
AudioLineInputController::reset()
{
//  if (this->lock_audio_writing()) {
    this->m_audio_input.reset();
    AudioInputController::reset();
/*    this->unlock_audio_writing();
  }
  else {
    fprintf(stderr, "AudioLineInputController::reset failed locking.\n");
  }//*/
}

unsigned long
AudioLineInputController::read_input()
{
  unsigned long ret_val = 0;
//  if (this->lock_audio_writing()) {
    ret_val = this->m_audio_input.read_input(this->m_audio_data);
/*    this->unlock_audio_writing();
  }
  else {
    fprintf(stderr, "ALIC:read_input failed locking.\n");
  }//*/
  return ret_val;
}
