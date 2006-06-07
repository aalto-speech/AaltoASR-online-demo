
#include <assert.h>
#include "AudioInput.hh"
#include "Buffer.hh"

AudioStream* AudioInput::m_audio_stream = NULL;
AudioBuffer* AudioInput::m_audio_buffer = NULL;
//unsigned int AudioInput::m_buffer_lock = 0;
pthread_mutex_t AudioInput::m_lock = PTHREAD_MUTEX_INITIALIZER;
unsigned long AudioInput::m_buffer_min_size = 100;

bool
AudioInput::initialize()
{
  m_audio_stream = new AudioStream();
  m_audio_buffer = new AudioBuffer(m_buffer_min_size);
  if (m_audio_stream == NULL || m_audio_buffer == NULL) {
    fprintf(stderr, "AudioInput::initialize out of memory.\n");
    return false;
  }
  
  if (!AudioStream::initialize())
    return false;
    
  if (!m_audio_stream->open(input_handler, NULL))
    return false;
    
  return true;
}

void
AudioInput::terminate()
{
  if (m_audio_stream) {
    m_audio_stream->close();
    AudioStream::terminate();
    delete m_audio_stream;
    m_audio_stream = NULL;
  }
  if (m_audio_buffer) {
    delete m_audio_buffer;
    m_audio_buffer = NULL;
  }
}

bool
AudioInput::start_input()
{
  assert(m_audio_stream && m_audio_buffer);
  return m_audio_stream->start();
}

void
AudioInput::stop_input()
{
  assert(m_audio_stream && m_audio_buffer);
  m_audio_stream->stop();
}

bool
AudioInput::read_block(AUDIO_FORMAT *to, unsigned long block_size)
{
  assert(m_audio_stream && m_audio_buffer);

/*
  m_buffer_lock++;
  if (m_buffer_lock != 1 || m_audio_buffer->get_read_size() < block_size) {
    m_buffer_lock--;
    return false;
  }
//*/
///*
  if (m_audio_buffer->get_read_size() < block_size)
    return false;
  pthread_mutex_lock(&m_lock);
//*/
  m_audio_buffer->read(to, block_size);
  pthread_mutex_unlock(&m_lock);
  //m_buffer_lock--;
  
  return true;
}

bool
AudioInput::check_buffer_size()
{
  assert(m_audio_stream && m_audio_buffer);
  
  unsigned long read_size = m_audio_buffer->get_read_size();
  unsigned long size = m_audio_buffer->get_size();
  
  bool ret = true;
/*  
  m_buffer_lock++;
  if (m_buffer_lock != 1) {
    m_buffer_lock--;
    return false;
  }
//*/
  pthread_mutex_lock(&m_lock);
  // Check if buffer is too full
  if (read_size > 0.6 * size) {
    ret = m_audio_buffer->resize(3*size);
  }
  // Check if buffer is too empty
  if (read_size < 0.1 * size && size > 2*m_buffer_min_size) {
//    ret = m_audio_buffer->resize(size/2);
  }
  pthread_mutex_unlock(&m_lock);
  
  //m_buffer_lock--;
  return ret;
}

bool
AudioInput::input_handler(const short *inputBuffer, short *outputBuffer,
                          unsigned long frameCount, void *userData)
{
  assert(m_audio_stream && m_audio_buffer);
/*  
  m_buffer_lock++;
  if (m_buffer_lock == 1) {
    m_audio_buffer->write(inputBuffer, frameCount);
  }
  m_buffer_lock--;
//*/
///*
  // vai trylock?
  if (pthread_mutex_lock(&m_lock) == 0) {
    m_audio_buffer->write(inputBuffer, frameCount);
    pthread_mutex_unlock(&m_lock);
  }
//*/
  // Return true means that this callback function will be called
  return true;
}

