
#ifndef AUDIOSTREAM_HH_
#define AUDIOSTREAM_HH_

#include <portaudio.h>
#include <string>
//#include "ErrorHandler.hh"

typedef short AUDIO_FORMAT;
#define audio_read_function sf_read_short
#define PA_AUDIO_FORMAT paInt16
const unsigned long SAMPLE_RATE = 16000;

namespace audio
{
  bool audio::read_wav_data(const char *filename, std::string *to);
};

class AudioStream//  :  public ErrorHandler
{
  
public:

  // debuggauksessa aikoinaan käytetty. voi poistaa!
//  static unsigned long luku;

  /** Initializes audio device. Call this before opening any streams. 
   * \return True for successful initialization. */
  static bool initialize();
  /** Terminates audio device. Call this after you don't need any streams. */
  static void terminate();

  AudioStream();
  virtual ~AudioStream();

  /** Opens the audio stream. */
  virtual bool open() = 0;
  virtual void close();
  
  /** Starts the audio stream, that is, starts a new thread for listening
   * audio device and calling the callback function. */
  virtual bool start();
  
protected:

  bool open_stream(unsigned int input_channels, unsigned int output_channels);

  virtual bool stream_callback(const AUDIO_FORMAT *input_buffer,
                               AUDIO_FORMAT *output_buffer,
                               unsigned long frame_count) = 0;

  static int callback(const void* input_buffer, void* output_buffer,
                      unsigned long frame_count,
                      const PaStreamCallbackTimeInfo *time_info,
                      PaStreamCallbackFlags status_flags, void* instance);

  static void print_error(const std::string &message, const PaError *error = NULL);

private:

  PaStream *m_stream;
  
};

#endif /*AUDIOSTREAM_HH_*/
