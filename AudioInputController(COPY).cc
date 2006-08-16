
#include "AudioInputController.hh"

AudioInputController::AudioInputController(msg::OutQueue *out_queue)
  : m_out_queue(out_queue), m_playback_buffer(32000)//, m_speakers(32000)
{
  this->m_recognizer_cursor = 0;
  this->m_audio_data.clear();

  this->m_paused = true;
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
  if (!this->m_audio_stream.open(true, true)) {
    fprintf(stderr, "Failed to open audio stream.\n");
    return false;
  }
  if (!this->m_audio_stream.start()) {
    fprintf(stderr, "AIC initialization failed to start audio stream.\n");
    return false;
  }
  return true;
//  return this->open_audio_stream();
  
  /*
  if (!AudioStream::initialize()) {
    fprintf(stderr, "AIC initialization failed: Audio stream initialization failed\n");
    return false;
  }
  
//  if (!this->open_stream(false)) {
  if (!this->open_stream()) {
    fprintf(stderr, "AIC initialization failed: Failed opening audio stream.\n");
    return false;
  }
  
  if (!this->m_audio_stream.start()) {
    fprintf(stderr, "AIC initialization failed to start audio stream.\n");
    return false;
  }
  //*/
}

void
AudioInputController::terminate()
{
  this->m_audio_stream.close();
//  this->close_audio_stream();
//  this->m_audio_stream.close();
//  this->m_audio_stream.close();
//  AudioStream::terminate();
}
/*
bool
AudioInputController::open_audio_stream()
{
  if (!this->m_audio_stream.open(true, true)) {
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
AudioInputController::close_audio_stream()
{
  this->m_audio_stream.close();
}
//*/
/*
bool
AudioInputController::open_stream()
{
  // NOTE: At first I opened only input or output streams if they were needed.
  // For several weeks I perceived a delay in output playback. The delay was
  // a bit less than second long. It bothered me but I couldn't fix it. One
  // day I happened to notice that when both input and output streams are open,
  // the output delay became obsolete. It sounds a bit weird to me and I don't
  // know why this is, but since then I have opened both input and output
  // because it seems to fix the "delay-bug".
  assert(false);
  return false;
//  return this->m_audio_stream.open(true, true);
}
//*/
/*
bool
AudioInputController::open_stream(bool input_stream)
{
  return this->m_audio_stream.open(input_stream, true);
}
//*/

unsigned long
AudioInputController::operate()
{
  msg::Message message(msg::M_AUDIO);
  const char *audio_data;
  unsigned long read_size = 0;

  // Do playback if playback is requested.  
  if (this->m_playback) {// && this->m_paused) {
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

      // Send message to out queue.
      this->m_out_queue->add_message(message);
//      this->m_out_queue->send_message(message);
    }
  }

  this->m_recognizer_cursor += read_size;

  return read_size;
}

void
AudioInputController::pause_listening(bool pause)
{
  //*
  if (pause) {
//    this->m_audio_stream.set_output_buffer(&this->m_playback_buffer);
    this->m_audio_stream.set_output_buffer(&this->m_playback_buffer);
  }
  else {
    // Disconnect the buffer before clearing.
//    this->m_audio_stream.set_output_buffer(NULL);
    this->m_audio_stream.set_output_buffer(NULL);
    this->stop_playback();
  }
  //*/
  this->m_paused = pause;
}

bool
AudioInputController::start_playback(unsigned long from, unsigned long length)
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
//    this->m_audio_stream.set_output_buffer(NULL);
    this->m_audio_stream.set_output_buffer(NULL);
    this->m_playback_buffer.clear();
//    this->m_audio_stream.set_output_buffer(&this->m_playback_buffer);
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
  this->m_audio_data.clear();
  this->m_playback_length = 0;
  this->m_playback_played = 0;
  this->m_playback = false;
  this->reset_cursors();
}

void
AudioInputController::reset_cursors()
{
  this->m_playback_buffer.clear();
  this->m_recognizer_cursor = 0;
}
