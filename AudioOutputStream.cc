
#include <assert.h>
//#include <string.h>
#include "AudioOutputStream.hh"
#include "Buffer.hh"

AudioOutputStream::AudioOutputStream(unsigned long buffer_size)
{
  this->m_frames_played = 0;
  this->m_paused = false;
  pthread_mutex_init(&this->m_lock, NULL);// PTHREAD_MUTEX_INITIALIZER;

  this->m_audio_buffer = new AudioBuffer(buffer_size);
  /*
  //throw exception???
  if (this->m_audio_buffer == NULL) {
    fprintf(stderr, "AudioOutput constructor out of memory.\n");
    return false;
  }
  //*/
}

AudioOutputStream::~AudioOutputStream()
{
  pthread_mutex_destroy(&this->m_lock);
}

bool
AudioOutputStream::open()
{
  this->m_frames_played = 0;
  // Open an audio stream with no input and one output channel.
  return this->open_stream(0, 1);
}

void
AudioOutputStream::close()
{
  AudioStream::close();
  if (this->m_audio_buffer) {
    delete this->m_audio_buffer;
    this->m_audio_buffer = NULL;
  }
}
/*
bool
AudioOutputStream::start()
{
  assert(this->m_audio_stream && this->m_audio_buffer);
  return this->m_audio_stream->start();
}
//*/
//*
void
AudioOutputStream::pause_output(bool pause)
{
//  assert(this->m_audio_stream && this->m_audio_buffer);
  this->m_paused = pause;
}
//*/

unsigned long
AudioOutputStream::write_output(const AUDIO_FORMAT *from, unsigned long size)
{
//  return this->write_output((char*)from, size);
//  assert(this->m_audio_stream && this->m_audio_buffer);
  unsigned long write_size;
  
  // Lock read/write-cursors before determining read size.
  pthread_mutex_lock(&this->m_lock);
  write_size = this->m_audio_buffer->get_write_size();
  pthread_mutex_unlock(&this->m_lock);
  
  if (write_size > size)
    write_size = size;
  
  // We can write without locking, because reading is done to different part
  write_size = this->m_audio_buffer->write(from, write_size);
  
  // Move read cursor
  pthread_mutex_lock(&this->m_lock);
  this->m_audio_buffer->move_write_pos(write_size);
  pthread_mutex_unlock(&this->m_lock);
  
  return write_size;
}
/*
unsigned long
AudioOutputStream::write_output(const char *from, unsigned long size)
{
//  assert(this->m_audio_stream && this->m_audio_buffer);
  unsigned long write_size;
  
  // Lock read/write-cursors before determining read size.
  pthread_mutex_lock(&this->m_lock);
  write_size = this->m_audio_buffer->get_write_size();
  pthread_mutex_unlock(&this->m_lock);
  
  if (write_size > size)
    write_size = size;
  
  // We can write without locking, because reading is done to different part
  write_size = this->m_audio_buffer->write(from, write_size);
  
  // Move read cursor
  pthread_mutex_lock(&this->m_lock);
  this->m_audio_buffer->move_write_pos(write_size);
  pthread_mutex_unlock(&this->m_lock);
  
  return write_size;
}
//*/
//*
void
AudioOutputStream::reset()
{
  if (pthread_mutex_lock(&this->m_lock) == 0) {
    this->m_audio_buffer->clear();
    this->m_frames_played = 0;
    pthread_mutex_unlock(&this->m_lock);
  }
}

bool
AudioOutputStream::stream_callback(const AUDIO_FORMAT *input_buffer,
                                   AUDIO_FORMAT *output_buffer,
                                   unsigned long frame_count)
{
  unsigned long read_size;

  if (!this->m_paused) {
    pthread_mutex_lock(&this->m_lock);
    read_size = this->m_audio_buffer->get_read_size();
    pthread_mutex_unlock(&this->m_lock);
  }
  else {
    read_size = 0;
  }

  // Prevent undefined scratching output..  
  if (frame_count > read_size) {
    memset(output_buffer + read_size, 0, sizeof(AUDIO_FORMAT) * (frame_count - read_size));
    frame_count = read_size;
  }

  if (frame_count) {
    this->m_audio_buffer->read(output_buffer, frame_count);

    pthread_mutex_lock(&this->m_lock);
    this->m_audio_buffer->move_read_pos(frame_count);
    pthread_mutex_unlock(&this->m_lock);
  }
  
  this->m_frames_played += frame_count;

  // Keep callbacking..
  return true;
}
