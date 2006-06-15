
#ifndef AUDIOSTREAM_HH_
#define AUDIOSTREAM_HH_

#include <portaudio.h>
#include <string>

typedef short AUDIO_FORMAT;
#define audio_read_function sf_read_short
#define PA_AUDIO_FORMAT paInt16
const unsigned long SAMPLE_RATE = 16000;

namespace audio
{
  bool audio::read_wav_data(const char *filename, std::string *to);
};

class AudioStream
{
  
public:

  // Call this before opening any streams.
  static bool initialize();
  // Call this after you don't need any streams.
  static void terminate();

  AudioStream();
  virtual ~AudioStream();

  virtual bool open() = 0;
  virtual void close();
  // Starts the audio stream.
  virtual bool start();
//  void stop();
//  void abort();
//  inline void pause(bool paused) { this->m_paused = paused; }


protected:

  bool open_stream(unsigned int input_channels, unsigned int output_channels);

  virtual bool stream_callback(const AUDIO_FORMAT *input_buffer,
                               AUDIO_FORMAT *output_buffer,
                               unsigned long frame_count) = 0;

  static int callback(const void* input_buffer, void* output_buffer,
                      unsigned long frame_count,
                      const PaStreamCallbackTimeInfo *time_info,
                      PaStreamCallbackFlags status_flags, void* instance);

  static void print_error(const char *message, const PaError *error);

private:

//  bool m_paused;
  PaStream *m_stream;
//  bool (*m_callback_function)(const AUDIO_FORMAT *inputBuffer,
//                              AUDIO_FORMAT *outputBuffer,
//                              unsigned long frameCount, void *userData);
//  void *m_callback_data;

};

#endif /*AUDIOSTREAM_HH_*/
