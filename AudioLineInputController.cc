
#include "AudioLineInputController.hh"

AudioLineInputController::AudioLineInputController(OutQueueController *out_queue)
  : AudioInputController(out_queue), m_input_buffer(160000)
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
/*    
  if (!this->m_audio_input.open()) {
    fprintf(stderr, "ALIC initialization failed: Couldn't initialize audio input.\n");
    return false;
  }
  if (!this->m_audio_input.start()) {
    fprintf(stderr, "ALIC.start_listening failed to start audio input.\n");
    return false;
  }
//*/
  return true;
}

void
AudioLineInputController::terminate()
{
//  this->m_audio_input.close();
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
  AudioInputController::pause_listening(pause);
  if (pause)
    this->m_audio_stream.set_input_buffer(NULL);
  else
    this->m_audio_stream.set_input_buffer(&this->m_input_buffer);

//  this->m_audio_input.pause_input(pause);
}

void
AudioLineInputController::reset_cursors()
{
//  this->m_audio_input.reset();
  this->m_input_buffer.clear();
  AudioInputController::reset_cursors();
}

unsigned long
AudioLineInputController::read_input()
{
//  fprintf(stderr, "ALIC:read_in start\n");
  this->m_input_buffer.read(this->m_audio_data);
//  fprintf(stderr, "ALIC:read_in end\n");
//  this->m_audio_input.read_input(this->m_audio_data);
  return this->get_audio_data_size() - this->get_read_cursor();
  /*
  unsigned long ret_val = 0;
  ret_val = this->m_audio_input.read_input(this->m_audio_data);
  return ret_val;
  //*/
}
