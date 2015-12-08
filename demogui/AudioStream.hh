
#ifndef AUDIOSTREAM_HH_
#define AUDIOSTREAM_HH_

#include <portaudio.h>
#include <string>
#include <pthread.h>
#include "Buffer.hh"

// These definitions make it easier to change audio format.
typedef short AUDIO_FORMAT;
#define audio_read_function sf_read_short
#define audio_write_function sf_write_short
#define PA_AUDIO_FORMAT paInt16

// This definition makes a bit nicer code.
typedef Buffer<AUDIO_FORMAT> AudioBuffer;

// I define these functions here because they need to know the audio format
// and do some own definitions acoording to that.
// Everything that has this kind of audio format dependency should be
// defined here to make it easier to change the audio format.
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

  extern unsigned int audio_sample_rate;
                      
};

/** Class for creating one audio stream using PortAudio inside. Only one stream
 * can be opened at a time so it is recommended to use only one instance
 * of this class in a program.
 * 
 * Stream uses buffers for both input and output. The stream runs after start
 * call until close is called. During that time it writes audio input into input
 * buffer and reads output buffer to output. If a stream is wanted to pause,
 * the buffer(s) should be disconnected (set to NULL). Thus input/output won't
 * communicate with the buffer anymore. Also the buffers can be changed at
 * anytime. */
class AudioStream
{
  
public:

  /** Constructs audio stream object. */
  AudioStream();
  /** Destructs the audio stream object. */
  ~AudioStream();

  /** Opens the audio stream.
   * \param input_stream true if input stream should be opened.
   * \param output_stream true if output stream should be opened.
   * \param select_devices true if the user should select input/output device.
   * \return false if failed to open the stream. */
  bool open(bool input_stream, bool output_stream, bool select_devices);

  /** Closes the audio stream. */
  void close();

  /** Starts the audio stream, that is, starts a new thread for listening
   * audio device and calling the callback function.
   * \return false if failed to start the stream. */
  bool start();

  /** Sets the input buffer for the audio stream. You can be sure that the
   * buffer is really changed after function returns.
   * \param input_buffer New input buffer. */
  void set_input_buffer(AudioBuffer *input_buffer);
  /** Sets the output buffer for the audio stream. You can be sure that the
   * buffer is really changed after function returns.
   * \param output_buffer New output buffer. */
  void set_output_buffer(AudioBuffer *output_buffer);
  /** \return Current input buffer. */
  inline AudioBuffer* get_input_buffer();
  /** \return Current output buffer. */
  inline AudioBuffer* get_output_buffer();

protected:

  /** Callback function for input stream.
   * \param input_buffer Buffer containing new audio input.
   * \param frame_count Number of audio samples. */
  void input_stream_callback(const AUDIO_FORMAT *input_buffer,
                             unsigned long frame_count);

  /** Callback function for output stream.
   * \param output_buffer Output should be written into this buffer.
   * \param frame_count Number of audio samples allowed to write. */
  void output_stream_callback(AUDIO_FORMAT *output_buffer,
                              unsigned long frame_count);

  /** Callback function for PortAudio.
   * \param input_buffer Buffer containing new audio input.
   * \param output_buffer Output should be written into this buffer.
   * \param frame_count Size (in samples) of both buffers.
   * \param time_info Some PortAudio time info.
   * \param status_flag Some PortAudio flags.
   * \param instance User data, in this case pointer to this object.
   * \return paContinue if stream wanted to stay active. */
  static int callback(const void* input_buffer,
                      void* output_buffer,
                      unsigned long frame_count,
                      const PaStreamCallbackTimeInfo *time_info,
                      PaStreamCallbackFlags status_flags,
                      void* instance);

  /** Print error text to stderr.
   * \param message Error message.
   * \param error Pointer to PortAudio's own error value. */
  static void print_error(const std::string &message,
                          const PaError *error = NULL);

private:

  PaStream *m_stream; //!< PortAudio audio stream.

  AudioBuffer *m_input_buffer; //!< Pointer to input buffer.
  AudioBuffer *m_output_buffer; //!< Pointer to output buffer.
  
  // Locks to prevent buffer change while writing/reading the buffer.
  pthread_mutex_t m_inputbuffer_lock; //!< Lock for input buffer changes.
  pthread_mutex_t m_outputbuffer_lock; //!< Lock for output buffer changes.
};

AudioBuffer*
AudioStream::get_input_buffer()
{
  return this->m_input_buffer;
}

AudioBuffer*
AudioStream::get_output_buffer()
{
  return this->m_output_buffer;
}

#endif /*AUDIOSTREAM_HH_*/
