
#ifndef AUDIOSTREAM_HH_
#define AUDIOSTREAM_HH_

#include <portaudio.h>
#include <string>
#include <pthread.h>
//#include <set>
#include "Buffer.hh"

// These definitions make it easier to change audio format etc.
typedef short AUDIO_FORMAT;
#define audio_read_function sf_read_short
#define audio_write_function sf_write_short
#define PA_AUDIO_FORMAT paInt16
const unsigned long SAMPLE_RATE = 16000;

// This definition makes a bit nicer code.
typedef Buffer<AUDIO_FORMAT> AudioBuffer;

namespace audio
{
  /** Reads the audio data from an audio file.
   * \param filename Audio file to read.
   * \param to String which the audio data is written into.
   * \return false if failed to read the audio file. */
  bool read_wav_data(const std::string &filename, std::string &to);

  /** Opens an audio file and writes audio samples into it.
   * \param filename Audio file to write.
   * \param from Array of audio samples.
   * \param frames Amount of audio samples.
   * \return false if failed write the audio file. */
  bool write_wav_data(const std::string &filename,
                      const AUDIO_FORMAT *from,
                      unsigned long frames);
};

class AudioStream
{
  
public:

  /** Initializes audio device. Call this before opening any streams. 
   * \return true for successful initialization. */
  static bool open(bool input_stream, bool output_stream);
  static void close();
  
  /** Starts the audio stream, that is, starts a new thread for listening
   * audio device and calling the callback function. */
  static bool start();

  // You can be sure that the buffer is actually changed after function returns.
  static void set_input_buffer(AudioBuffer *input_buffer);
  static void set_output_buffer(AudioBuffer *output_buffer);
  static inline const AudioBuffer* get_input_buffer();
  static inline const AudioBuffer* get_output_buffer();

protected:

  static void input_stream_callback(const AUDIO_FORMAT *input_buffer,
                                    unsigned long frame_count);

  static void output_stream_callback(AUDIO_FORMAT *output_buffer,
                                     unsigned long frame_count);

  static int callback(const void* input_buffer,
                      void* output_buffer,
                      unsigned long frame_count,
                      const PaStreamCallbackTimeInfo *time_info,
                      PaStreamCallbackFlags status_flags,
                      void* user_data);

  static void print_error(const std::string &message,
                          const PaError *error = NULL);

private:

  static PaStream *m_stream;
//  static std::set<AudioStream*> m_all_streams;

  static bool m_input_stream;
  static bool m_output_stream;
  static AudioBuffer *m_input_buffer;
  static AudioBuffer *m_output_buffer;
  
  static pthread_mutex_t m_inputbuffer_lock;
  static pthread_mutex_t m_outputbuffer_lock;

};

const AudioBuffer*
AudioStream::get_input_buffer()
{
  return this->m_input_buffer;
}

const AudioBuffer*
AudioStream::get_output_buffer()
{
  return this->m_output_buffer;
}

#endif /*AUDIOSTREAM_HH_*/
