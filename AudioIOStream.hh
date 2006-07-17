
#ifndef AUDIOIOSTREAM_HH_
#define AUDIOIOSTREAM_HH_

#include "AudioInputStream.hh"
#include "AudioOutputStream.hh"

class AudioIOStream  :  public AudioInputStream, public AudioOutputStream
{
  
public:

  virtual bool open();

protected:

  virtual bool stream_callback(const AUDIO_FORMAT *input_buffer,
                               AUDIO_FORMAT *output_buffer,
                               unsigned long frame_count);
};

#endif /*AUDIOIOSTREAM_HH_*/
