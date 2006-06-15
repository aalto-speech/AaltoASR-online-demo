
#include <assert.h>
#include "AudioInputStream.hh"

AudioInputStream::AudioInputStream(unsigned long buffer_size)
{
  this->m_audio_buffer = NULL;
  this->m_paused = false;
  pthread_mutex_init(&this->m_lock, NULL);

  this->m_audio_buffer = new AudioBuffer(buffer_size);
  // TODO: Throw an exception here if failed???
//  if (this->m_audio_stream == NULL || this->m_audio_buffer == NULL) {
//    fprintf(stderr, "AudioInput::open out of memory.\n");
//    return false;
//  }
  
}

AudioInputStream::~AudioInputStream()
{
  pthread_mutex_destroy(&this->m_lock);
}

bool
AudioInputStream::open()
{
  // Open an audio stream with one input and no output channels.
  return this->open_stream(1, 0);
}

// TÄNNE JÄIN HOMMIIIN!!!!!!

void
AudioInputStream::close()
{
  AudioStream::close();
  if (this->m_audio_buffer) {
    delete this->m_audio_buffer;
    this->m_audio_buffer = NULL;
  }
}
/*
bool
AudioInputStream::start()
{
  assert(this->m_audio_stream && this->m_audio_buffer);
  return this->m_audio_stream->start();
}
//*/
void
AudioInputStream::pause_input(bool pause)
{
//  assert(this->m_audio_stream && this->m_audio_buffer);
  this->m_paused = pause;
}

unsigned long
AudioInputStream::read_input(AUDIO_FORMAT *to)
{
//  assert(this->m_audio_stream && this->m_audio_buffer);

  unsigned long read_size;
  
  // Lock read/write-cursors before determining read size.
  pthread_mutex_lock(&this->m_lock);
  read_size = this->m_audio_buffer->get_read_size();
  pthread_mutex_unlock(&this->m_lock);
  
  // We can read without locking, because writing is done to different part
  read_size = this->m_audio_buffer->read(to, read_size);
  
  // Move read cursor
  pthread_mutex_lock(&this->m_lock);
  this->m_audio_buffer->move_read_pos(read_size);
  pthread_mutex_unlock(&this->m_lock);
  
  return read_size;
}

unsigned long
AudioInputStream::read_input(std::string &to)
{
//  assert(this->m_audio_stream && this->m_audio_buffer);

  unsigned long read_size;
  
  // Lock read/write-cursors before determining read size.
  pthread_mutex_lock(&this->m_lock);
  read_size = this->m_audio_buffer->get_read_size();
  pthread_mutex_unlock(&this->m_lock);
  
  // We can read without locking, because writing is done to different part
  read_size = this->m_audio_buffer->read(to, read_size);
  
  // Move read cursor
  pthread_mutex_lock(&this->m_lock);
  this->m_audio_buffer->move_read_pos(read_size);
  pthread_mutex_unlock(&this->m_lock);
  
  return read_size;
}

bool
AudioInputStream::stream_callback(const AUDIO_FORMAT *input_buffer,
                                  AUDIO_FORMAT *output_buffer,
                                  unsigned long frame_count)
{
  unsigned long write_size;
//  AudioInputStream *object = (AudioInputStream*)user_data;

//  assert(object->m_audio_stream && object->m_audio_buffer);

  if (this->m_paused)
    return true;

  pthread_mutex_lock(&this->m_lock);
  write_size = this->m_audio_buffer->get_write_size();
  pthread_mutex_unlock(&this->m_lock);
  
  if (frame_count > write_size) {
    // In this case we lose data: enlarge buffer?
    assert(false);
    frame_count = write_size;
  }

  this->m_audio_buffer->write(input_buffer, frame_count);

  pthread_mutex_lock(&this->m_lock);
  this->m_audio_buffer->move_write_pos(frame_count);
  pthread_mutex_unlock(&this->m_lock);

  // Keep callbacking..
  return true;
}
