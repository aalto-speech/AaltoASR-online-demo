
#ifndef AUDIOSTREAM_HH_
#define AUDIOSTREAM_HH_

#include <portaudio.h>

#define AUDIO_FORMAT short
#define PA_AUDIO_FORMAT paInt16
#define SAMPLE_RATE 16000

class AudioStream
{

public:

  AudioStream();

  // Closes the audio stream.
  ~AudioStream();

  // Call this before opening any streams.
  static bool initialize();
  // Call this after you don't need any streams.
  static void terminate();

  bool open(bool (*callback_function)(const AUDIO_FORMAT *inputBuffer,
                                      AUDIO_FORMAT *outputBuffer,
                        				      unsigned long frameCount, void *userData),
      	    void *callback_data);
  void close();

  // Activates the audio stream.
  bool start();
  void stop();
  void abort();


protected:

  static int callback(const void* inputBuffer, void* outputBuffer,
	                    unsigned long frameCount,
                      const PaStreamCallbackTimeInfo *timeInfo,
                      PaStreamCallbackFlags statusFlags, void* instance);

  static void print_error(const char *message, const PaError *error);
  

  PaStream *m_stream;
  bool (*m_callback_function)(const AUDIO_FORMAT *inputBuffer, AUDIO_FORMAT *outputBuffer,
			                        unsigned long frameCount, void *userData);
  void *m_callback_data;

};

#endif
