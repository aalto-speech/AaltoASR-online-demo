
#include "AudioInputController.hh"

AudioInputController::AudioInputController(OutQueueController *out_queue)
  : m_out_queue(out_queue), m_output_buffer(32000)//, m_speakers(32000)
{
  this->m_recognizer_cursor = 0;
  this->m_audio_data.clear();

  this->m_playback = false;
  this->m_playback_from = 0;
  this->m_playback_length = 0;
  this->m_playback_played = 0;
}

AudioInputController::~AudioInputController()
{
}

bool
AudioInputController::initialize()
{
  if (!AudioStream::initialize()) {
    fprintf(stderr, "AIC initialization failed: Audio stream initialization failed\n");
    return false;
  }
  
  if (!this->open_stream(false)) {
    fprintf(stderr, "AIC initialization failed: Couldn't initialize audio output.\n");
    return false;
  }
  if (!this->m_audio_stream.start()) {
    fprintf(stderr, "AIC initialization failed to start audio output.\n");
    return false;
  }
  
//  this->m_audio_stream.set_output_buffer(&this->m_output_buffer);

  return true;
}

bool
AudioInputController::open_stream(bool input_stream)
{
  return this->m_audio_stream.open(input_stream, true);
}

void
AudioInputController::terminate()
{
  this->m_audio_stream.close();
  AudioStream::terminate();
}

void
AudioInputController::operate()
{
  static msg::Message message(msg::M_AUDIO);
  static const char *audio_data;
  static unsigned long read_size;
  
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
    this->m_playback_played += this->m_output_buffer.write(this->get_audio_data() + this->m_playback_from + this->m_playback_played,
                                                           write_size - this->m_playback_played);
  }
  
  this->read_input();
  read_size = this->get_audio_cursor() - this->m_recognizer_cursor;
//  fprintf(stderr, "Listen read size: %d\n", read_size);

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

      this->m_recognizer_cursor += read_size;

      // Send message to out queue.
      this->m_out_queue->send_message(message);
    }
  }
  else {
    this->m_recognizer_cursor += read_size;
  }
}

void
AudioInputController::pause_listening(bool pause)
{
  if (pause) {
    this->m_audio_stream.set_output_buffer(&this->m_output_buffer);
  }
  else {
    // Disconnect the buffer before clearing.
    this->m_audio_stream.set_output_buffer(NULL);
    this->m_output_buffer.clear();
  }
}

void
AudioInputController::play(unsigned long from, unsigned long length)
{
  /*
  if (this->m_audio_stream.get_output_buffer() == &this->m_output_buffer) {
    this->m_audio_stream.set_output_buffer(NULL);
    this->m_output_buffer.clear();
    this->m_audio_stream.set_output_buffer(&this->m_output_buffer);
  }
  else {
    this->m_output_buffer.clear();
  }
  //*/
  this->m_playback = true;
  this->m_playback_from = from;
  this->m_playback_length = length;
  this->m_playback_played = 0;
}

void
AudioInputController::stop()
{
  if (this->m_audio_stream.get_output_buffer() == &this->m_output_buffer) {
    this->m_audio_stream.set_output_buffer(NULL);
    this->m_output_buffer.clear();
    this->m_audio_stream.set_output_buffer(&this->m_output_buffer);
  }
  else {
    this->m_output_buffer.clear();
  }
  this->m_playback = false;
}

void
AudioInputController::reset()
{
  this->m_audio_data.clear();
  this->reset_cursors();
}

void
AudioInputController::reset_cursors()
{
  this->m_output_buffer.clear();
  this->m_recognizer_cursor = 0;
}

/*
unsigned long
AudioInputController::get_sample_rate() const
{
  return SAMPLE_RATE;
}

unsigned int
AudioInputController::get_bytes_per_sample() const
{
  return sizeof(AUDIO_FORMAT);
}
//*/
