#ifndef AUDIOOUTPUTSTREAM_HH_
#define AUDIOOUTPUTSTREAM_HH_

#include <pthread.h>
#include "AudioStream.hh"
#include "Buffer.hh"

// NOTICE THIS DEFINITION!
typedef Buffer<AUDIO_FORMAT> AudioBuffer;

class AudioOutputStream  :  public AudioStream
{

public:

  AudioOutputStream(unsigned long buffer_size);
  virtual ~AudioOutputStream();

  virtual bool open();
  virtual void close();
//  virtual bool start();
  void pause_output(bool pause);
  
  inline unsigned long get_frames_played() const { return this->m_frames_played; }

  unsigned long write_output(const AUDIO_FORMAT *from, unsigned long size);
  // TODO: tämä funktio on itseasiassa turha.. poista!
  unsigned long write_output(const char *from, unsigned long size);

protected:

  bool stream_callback(const AUDIO_FORMAT *input_buffer,
                       AUDIO_FORMAT *output_buffer,
                       unsigned long frame_count);

private:

  AudioBuffer *m_audio_buffer;
  pthread_mutex_t m_lock;
  bool m_paused;
  unsigned long m_frames_played;
};

#endif /*AUDIOOUTPUTSTREAM_HH_*/
