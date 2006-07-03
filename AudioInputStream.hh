
#ifndef AUDIOINPUT_HH_
#define AUDIOINPUT_HH_

#include <pthread.h>
#include "AudioStream.hh"
#include "Buffer.hh"

// NOTICE THIS DEFINITION!
typedef Buffer<AUDIO_FORMAT> AudioBuffer;

/**
 * Buffered audio input stream.
 */
class AudioInputStream  :  public AudioStream
{

public:

  AudioInputStream(unsigned long buffer_size);
  ~AudioInputStream();

  virtual bool open();
  virtual void close();
  void pause_input(bool pause);

  unsigned long read_input(AUDIO_FORMAT *to);
  unsigned long read_input(std::string &to);
  
  void reset();

private:

  virtual bool stream_callback(const AUDIO_FORMAT *input_buffer,
                               AUDIO_FORMAT *output_buffer,
                               unsigned long frame_count);

  AudioBuffer *m_audio_buffer;
  pthread_mutex_t m_lock;
  bool m_paused;
};

#endif
