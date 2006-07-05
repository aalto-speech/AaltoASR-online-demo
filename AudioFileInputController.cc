
#include "AudioInputStream.hh"
#include "AudioFileInputController.hh"

AudioFileInputController::AudioFileInputController(OutQueueController *out_queue)
  : AudioInputController(out_queue), m_audio_output(48000)
{
  this->m_output_cursor = 0;
//  this->m_sent_eof = false;
//  this->m_eof = true;
}

AudioFileInputController::~AudioFileInputController()
{
}

bool
AudioFileInputController::load_file(const char *filename)
{
  bool ret_val = false;
//  if (this->lock_audio_writing()) {
//    this->reset();
    if (audio::read_wav_data(filename, &this->m_audio_data)) {
//      this->check_eof();
      ret_val = true;
    }
    else {
      fprintf(stderr, "AudioFileInputController::load_file failed.\n");
      this->reset();
      ret_val = false;
    }
/*    this->unlock_audio_writing();
  }//*/
  return ret_val;
}

void
AudioFileInputController::reset()
{
//  if (this->lock_audio_writing()) {
    this->m_audio_output.reset();
    AudioInputController::reset();
    this->m_output_cursor = 0;
//    this->check_eof();
//    this->m_eof = true;
/*    this->unlock_audio_writing();
  }
  else {
    fprintf(stderr, "AudioFileInputController::reset failed locking.\n");
  }//*/
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
/*
bool
AudioFileInputController::start_listening()
{
  // TODO: Tämä ennen tuota startia???
  this->m_output_cursor = 0;
  return AudioInputController::start_listening();
}
//*/
//*
void
AudioFileInputController::pause_listening(bool pause)
{
  this->m_audio_output.pause_output(pause);
//  AudioInputController::pause_listening(pause);
}
//*/

unsigned long
AudioFileInputController::read_input()
{
  /*
  if (this->is_eof()) {
    if (!this->m_sent_eof) {
      if (this->m_out_queue) {
        msg::Message message(msg::M_AUDIO_END);
        this->m_out_queue->send_message(message);
      }
      this->m_sent_eof = true;
//      this->stop_listening();
    }
    return 0;
  }
  else {
    this->m_sent_eof = false;
    //*/
//  if (!this->is_eof()){// && this->lock_audio_writing()) {
    // Send audio to output stream.
    this->m_output_cursor += this->m_audio_output.write_output((AUDIO_FORMAT*)this->m_audio_data.data() + this->m_output_cursor,
                                                              this->m_audio_data.length() / sizeof(AUDIO_FORMAT) - this->m_output_cursor);
    
//    this->unlock_audio_writing();

//  }
//  fprintf(stderr, "Frame count: %d\n", this->m_audio_output.luku);
  // Return the frames that have been played (in speakers) since last read.
  // This way we will synchronize recognizer with the sound from speakers.
    return this->m_audio_output.get_frames_played() - this->get_read_cursor();
//  }
}
