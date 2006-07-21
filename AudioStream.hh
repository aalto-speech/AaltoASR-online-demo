
#ifndef AUDIOSTREAM_HH_
#define AUDIOSTREAM_HH_

#include <portaudio.h>
#include <string>
#include <pthread.h>
#include <set>
#include "Buffer.hh"

typedef short AUDIO_FORMAT;
#define audio_read_function sf_read_short
#define audio_write_function sf_write_short
#define PA_AUDIO_FORMAT paInt16
const unsigned long SAMPLE_RATE = 16000;

// NOTICE THIS DEFINITION!
typedef Buffer<AUDIO_FORMAT> AudioBuffer;

namespace audio
{
  bool read_wav_data(const std::string &filename, std::string &to);
  bool write_wav_data(const std::string &filename, const AUDIO_FORMAT *from, unsigned long frames);
};

class AudioStream
{
  
public:

  /** Initializes audio device. Call this before opening any streams. 
   * \return True for successful initialization. */
  static bool initialize();
  /** Terminates audio device. Call this after you don't need any streams. */
  static void terminate();
  
  static void close_all_streams();

  AudioStream();
  ~AudioStream();

  /** Opens the audio stream. */
  bool open(bool input_stream, bool output_stream);
//  virtual bool open() = 0;
  void close();
  
//  void reset();
  
  /** Starts the audio stream, that is, starts a new thread for listening
   * audio device and calling the callback function. */
  bool start();

  // You can be sure that the buffer is actually changed after function returns.
  void set_input_buffer(AudioBuffer *input_buffer);// { this->m_input_buffer = input_buffer; }
  void set_output_buffer(AudioBuffer *output_buffer);// { this->m_output_buffer = output_buffer; }
  inline const AudioBuffer* get_input_buffer() const { return this->m_input_buffer; }
  inline const AudioBuffer* get_output_buffer() const { return this->m_output_buffer; }
//  inline unsigned long get_frames_played() const { return this->m_frames_played; }

protected:

//  bool open_stream(unsigned int input_channels, unsigned int output_channels);

  void input_stream_callback(const AUDIO_FORMAT *input_buffer,
                             unsigned long frame_count);

  void output_stream_callback(AUDIO_FORMAT *output_buffer,
                              unsigned long frame_count);

  static int callback(const void* input_buffer, void* output_buffer,
                      unsigned long frame_count,
                      const PaStreamCallbackTimeInfo *time_info,
                      PaStreamCallbackFlags status_flags, void* instance);

  static void print_error(const std::string &message, const PaError *error = NULL);

private:

  PaStream *m_stream;
  static std::set<AudioStream*> m_all_streams;

  bool m_input_stream;
  bool m_output_stream;
  AudioBuffer *m_input_buffer;
  AudioBuffer *m_output_buffer;
  
  pthread_mutex_t m_inputbuffer_lock;
  pthread_mutex_t m_outputbuffer_lock;
//  unsigned long m_frames_played;
};

/*
class AudioStream//  :  public ErrorHandler
{
  
public:

  // debuggauksessa aikoinaan käytetty. voi poistaa!
//  static unsigned long luku;

  static bool initialize();
  static void terminate();

  AudioStream();
  virtual ~AudioStream();

  virtual bool open() = 0;
  virtual void close();
  
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
//*/

#endif /*AUDIOSTREAM_HH_*/
