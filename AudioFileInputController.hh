
#ifndef AUDIOFILEINPUTCONTROLLER_HH_
#define AUDIOFILEINPUTCONTROLLER_HH_

#include "AudioInputController.hh"

/**
 * Class for handling operations between audio stream and out queue (pipe to
 * recognizer). Stores the whole audio data. Uses a specified audio file as
 * the source.
 */
class AudioFileInputController  :  public AudioInputController
{
  
public:

  /** Constructs an audio input controller.
   * \param out_queue The out queue which the audio messages should be sent
   *                  to. */
  AudioFileInputController(msg::OutQueue *out_queue);

  /** Destructs the controller. */
  virtual ~AudioFileInputController();

  /** Reads the audio data from an audio file into own audio data array.
   * \param filename Audio file to read.
   * \return false if failed to read the file. */  
  bool load_file(const std::string &filename);
  
  /** Resets cursors thus starting to send audio messages from the beginning. */
  virtual void reset_cursors();

  /** \return The index of last audio sample played in output channel, thus
   *          synchronizing the heard audio and out queue. */
  virtual unsigned long get_audio_cursor() const;

  /** Pauses (or unpauses) audio input reading. Only in paused state is playback
   * allowed. Disconnects (or connects) output buffer from audio stream.
   * \param pause True for pause, false for unpause. */
  virtual void pause_listening(bool pause);
  
  /** \return true if all the audio data has been read (=sent to out queue). */
  inline bool is_eof() const;

protected:

  /** Writes audio samples to output buffer so audio stream will play them. */
  virtual void read_input();
  
private:

  /** Output buffer for audio stream. */
  AudioBuffer m_output_buffer;
  /** Index of last audio sample written to output buffer. */
  unsigned long m_output_cursor; 
  
};

bool
AudioFileInputController::is_eof() const
{
  return this->get_read_cursor() >= this->get_audio_data_size();
}

#endif /*AUDIOFILEINPUTCONTROLLER_HH_*/
