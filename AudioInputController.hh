
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

  enum Mode { RECORD, PLAY };

  /** Reads the audio data from an audio file into own audio data array.
   * \param filename Audio file to read.
   * \return false if failed to read the file. */  
  bool load_file(const std::string &filename);

  /** Set mode. */  
  void set_mode(Mode mode);
  
  /** \param mute When mute is set to true, no output is played whatsoever. */
  void set_mute(bool mute);

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

  /** \return Audio samples (=audio data) as an array. */
  inline const AUDIO_FORMAT* get_audio_data() const;
  /** \return The amount of audio samples (=size of audio data array). */
  inline unsigned long get_audio_data_size() const;

  /** \return The amount of audio frames sent to out queue. */
  inline unsigned long get_read_cursor() const;
  /** \return The index of last audio sample allowed to send out queue. */
  inline unsigned long get_audio_cursor() const;
  
  inline bool is_eof() const;
  
protected:

  /** Reads audio samples from some source. */
  inline void read_input();
  
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
  unsigned long m_output_cursor;

  // Variables for playback.
  AudioBuffer m_playback_buffer;
  bool m_playback; //!< Tells if we are playing playback.
  unsigned long m_playback_from;
  unsigned long m_playback_length;
  unsigned long m_playback_played; //!< Frames written to output buffer.

  AudioBuffer m_output_buffer;
  AudioBuffer m_input_buffer;
  Mode m_mode;

  bool m_paused;
  bool m_mute;

};

//*
bool
AudioInputController::is_eof() const
{
  if (this->m_mode == PLAY)
    return this->get_read_cursor() >= this->get_audio_data_size();
  
  return false;
}
//*/

bool
AudioInputController::is_paused() const
{
  return this->m_paused;
}

unsigned long
AudioInputController::get_audio_cursor() const
{
  if (this->m_mode == PLAY)
    return this->m_output_buffer.get_frames_read();
  else // this->m_mode == RECORD
    return this->get_audio_data_size();
}

void
AudioInputController::read_input()
{
  if (this->m_mode == PLAY) {
    // Send audio to output stream.
    this->m_output_cursor +=
      this->m_output_buffer.write(this->get_audio_data() + this->m_output_cursor,
                                  this->get_audio_data_size() - this->m_output_cursor);
  }
  else { // this->m_mode == RECORD
    this->m_input_buffer.read(this->m_audio_data);
  }
}  

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
