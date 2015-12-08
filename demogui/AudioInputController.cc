
#include "AudioInputController.hh"

AudioInputController::AudioInputController(msg::OutQueue *out_queue)
  : m_out_queue(out_queue),
    m_playback_buffer(32000),
    m_output_buffer(48000),
    m_input_buffer(48000),
    m_mode(RECORD)
{
  this->m_recognizer_cursor = 0;
  this->m_output_cursor = 0;
  
  this->m_audio_data.clear();
  
  this->m_mute = false;

  this->m_paused = true;
  this->m_playback = false;
  this->m_playback_from = 0;
  this->m_playback_length = 0;
  this->m_playback_played = 0;
}

bool
AudioInputController::initialize()
{
  if (!this->m_audio_stream.open(true, true, false)) {
    fprintf(stderr, "Failed to open audio stream.\n");
    return false;
  }
  if (!this->m_audio_stream.start()) {
    fprintf(stderr, "AIC initialization failed to start audio stream.\n");
    return false;
  }
  return true;
}

void
AudioInputController::terminate()
{
  this->m_audio_stream.close();
}

bool
AudioInputController::load_file(const std::string &filename)
{
  if (!audio::read_wav_data(filename, this->m_audio_data)) {
    fprintf(stderr, "AudioFileInputController::load_file failed.\n");
    return false;
  }
  this->reset_cursors();
  return true;
}


unsigned long
AudioInputController::operate()
{
  msg::Message message(msg::M_AUDIO);
  const char *audio_data;
  unsigned long read_size = 0;

  // Do playback if playback is requested.  
  if (this->m_playback) {
    unsigned long write_size;
    unsigned long max_size = 0;

    if (this->get_audio_data_size() > this->m_playback_from)
      max_size = this->get_audio_data_size() - this->m_playback_from;

    write_size = max_size;
    if (this->m_playback_length) {
      assert(this->m_playback_length >= this->m_playback_played);
      if (this->m_playback_length < max_size)
        write_size = this->m_playback_length;
    }
    this->m_playback_played += this->m_playback_buffer.write(this->get_audio_data() + this->m_playback_from + this->m_playback_played,
                                                           write_size - this->m_playback_played);

    if (this->m_playback_buffer.get_frames_read() >= write_size) {
      this->stop_playback();
    }
                                                           
  }
  
  // Read microphone input or put audio to output.
  this->read_input();
  read_size = this->get_audio_cursor() - this->m_recognizer_cursor;

  if (this->m_out_queue) {
    // Send audio in max 6000 frame messages.
    if (read_size > 6000)
      read_size = 6000;
        
    if (read_size) {
      // Clear previous audio data and make message of the new data.
      message.clear_data();
      // Write new data.
      audio_data = this->m_audio_data.data();
      message.append(&audio_data[this->m_recognizer_cursor*sizeof(AUDIO_FORMAT)],
                     read_size * sizeof(AUDIO_FORMAT));

      // Send message to out queue. (do not flush)
      this->m_out_queue->add_message(message);
    }
  }

  this->m_recognizer_cursor += read_size;
  return read_size;
}

void
AudioInputController::set_mode(Mode mode)
{
  this->m_mode = mode;
  this->pause_listening(this->m_paused);
}

void
AudioInputController::set_mute(bool mute)
{
  if (mute) {
    this->m_audio_stream.set_output_buffer(NULL);
  }
  else {
    if (this->m_paused)
      this->m_audio_stream.set_output_buffer(&this->m_playback_buffer);
    else
      this->m_audio_stream.set_output_buffer(&this->m_output_buffer);
  }
  this->m_mute = mute;
}


void
AudioInputController::pause_listening(bool pause)
{
  if (pause) {
    this->m_audio_stream.set_input_buffer(NULL);
    if (!this->m_mute)
      this->m_audio_stream.set_output_buffer(&this->m_playback_buffer);
  }
  else {
    if (this->m_mode == PLAY) {
      this->m_audio_stream.set_input_buffer(NULL);
      if (!this->m_mute)
        this->m_audio_stream.set_output_buffer(&this->m_output_buffer);
    }
    else { // this->m_mode == RECORD
      this->m_audio_stream.set_input_buffer(&this->m_input_buffer);
      this->m_audio_stream.set_output_buffer(NULL);
    }
    this->stop_playback();
  }

  this->m_paused = pause;
}

bool
AudioInputController::start_playback(unsigned long from, unsigned long length)
{
  // Start playback only if paused and not already playbacking.
  if (this->m_paused && !this->m_playback) {
    this->m_playback = true;
    this->m_playback_from = from;
    this->m_playback_length = length;
    this->m_playback_played = 0;
    return true;
  }
  return false;
}

void
AudioInputController::stop_playback()
{
  if (this->m_audio_stream.get_output_buffer() == &this->m_playback_buffer) {
    this->m_audio_stream.set_output_buffer(NULL);
    this->m_playback_buffer.clear();
    this->m_audio_stream.set_output_buffer(&this->m_playback_buffer);
  }
  else {
    this->m_playback_buffer.clear();
  }
  this->m_playback = false;
}

void
AudioInputController::reset()
{
  // These are for thread safety.
  AudioBuffer *input_buffer = this->m_audio_stream.get_input_buffer();
  AudioBuffer *output_buffer = this->m_audio_stream.get_output_buffer();
  this->m_audio_stream.set_input_buffer(NULL);
  this->m_audio_stream.set_output_buffer(NULL);

  // Resetting.
  this->m_audio_data.clear();
  this->m_playback_length = 0;
  this->m_playback_played = 0;
  this->m_playback = false;
  this->m_playback_buffer.clear();
  this->reset_cursors();

  // ... for thread safety
  this->m_audio_stream.set_input_buffer(input_buffer);
  this->m_audio_stream.set_output_buffer(output_buffer);
}

void
AudioInputController::reset_cursors()
{
  // These are for thread safety.
  AudioBuffer *input_buffer = this->m_audio_stream.get_input_buffer();
  AudioBuffer *output_buffer = this->m_audio_stream.get_output_buffer();
  this->m_audio_stream.set_input_buffer(NULL);
  this->m_audio_stream.set_output_buffer(NULL);

  // Resetting.
  this->m_input_buffer.clear();
  this->m_output_buffer.clear();
  this->m_output_cursor = 0;
  this->m_recognizer_cursor = 0;

  // ... for thread safety
  this->m_audio_stream.set_input_buffer(input_buffer);
  this->m_audio_stream.set_output_buffer(output_buffer);
}
