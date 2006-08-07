
#ifndef AUDIOINPUTCONTROLLER_HH_
#define AUDIOINPUTCONTROLLER_HH_

#include <string>
#include "AudioStream.hh"
#include "msg.hh"

/**
 * Class for handling operations between audio stream and out queue (pipe to
 * recognizer). Stores the whole audio data.
 */
class AudioInputController
{
public:

  /** Constructs an audio input controller.
   * \param out_queue The out queue which the audio messages should be sent
   *                  to. */
  AudioInputController(msg::OutQueue *out_queue);
  
  /** Destructs the controller. */
  virtual ~AudioInputController();

  /** Creates audio stream.
   * \return false if failed to activate the audio stream for some reason. */
  bool initialize();
  /** Closes the audio stream. */
  void terminate();

  /** Reads new audio input and sends it as messages to out queue.
   * \return The amount of audio samples sent to out queue. */
  unsigned long operate();
  /** Pauses (or unpauses) audio input reading. Only in paused state is playback
   * allowed.
   * \param pause True for pause, false for unpause. */
  virtual void pause_listening(bool pause);
  /** \return Tells whether paused or not. */
  inline bool is_paused() const;
  
  /** Requests to start playback.
   * \param from Index of audio sample which to start playing from.
   * \param length Amount of audio samples to play or zero to play to the end.
   * \return false if listening is not paused or already playbacking.*/
  bool start_playback(unsigned long from, unsigned long length = 0);
  /** Stops playback. */
  void stop_playback();
  /** \return Tells whether currently playing playback. */
  inline bool is_playbacking() const;
  /** \return The index of the audio sample currently playbacking. */
  inline unsigned long get_playback_cursor() const;

  /** Resets cursors and audio data. */
  void reset();
  /** Resets cursors thus starting to send audio messages from the beginning. */
  virtual void reset_cursors();

  /** Opens audio stream. Stream is opened by default and this function is not
   * not needed from outside unless close_audio_stream is explicitly used.
   * \return true if opening is successful. */  
//  bool open_audio_stream();
  /** Closes audio stream. Normally there's no need to close the stream from
   * outside. */
//  void close_audio_stream();

  /** \return Audio samples (=audio data) as an array. */
  inline const AUDIO_FORMAT* get_audio_data() const;
  /** \return The amount of audio samples (=size of audio data array). */
  inline unsigned long get_audio_data_size() const;

  /** \return The amount of audio frames sent to out queue. */
  inline unsigned long get_read_cursor() const;
  /** \return The index of last audio sample allowed to send out queue. */
  virtual unsigned long get_audio_cursor() const = 0;
  
protected:

  /** Reads audio samples from some source. */
  virtual void read_input() = 0;
  
  /** Opens an audio stream. Opens both input and output streams (no matter
   * used or not) because it seemed to fix the audio delay bug.
   * \return False if failed for some reason. */
  bool open_stream();
  
  /** Audio samples stored as a string. Remember to type cast the array
   * into AUDIO_FORMAT array when modifying/reading samples. */
  std::string m_audio_data;
  
  /** Out queue which the audio messages are sent to. */
  msg::OutQueue *m_out_queue;
  
  /** Audio stream. */
  AudioStream m_audio_stream;
  
private:

  /** Index of the last audio sample sent as an audio message. */
  unsigned long m_recognizer_cursor;

  // Variables for playback.
  AudioBuffer m_playback_buffer;
  bool m_playback; //!< Tells if we are playing playback.
  unsigned long m_playback_from;
  unsigned long m_playback_length;
  unsigned long m_playback_played; //!< Frames written to output buffer.

  bool m_paused;

};

bool
AudioInputController::is_playbacking() const
{
  return this->m_playback;
}

unsigned long
AudioInputController::get_playback_cursor() const
{
  return this->m_playback_from + this->m_playback_buffer.get_frames_read();
}

const AUDIO_FORMAT*
AudioInputController::get_audio_data() const
{
  return (AUDIO_FORMAT*)this->m_audio_data.data();
}

unsigned long
AudioInputController::get_audio_data_size() const
{
  return this->m_audio_data.size() / sizeof(AUDIO_FORMAT);
}

unsigned long
AudioInputController::get_read_cursor() const
{
  return this->m_recognizer_cursor;
}

#endif /*AUDIOINPUTCONTROLLER_HH_*/
