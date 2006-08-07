
#ifndef AUDIOLINEINPUTCONTROLLER_HH_
#define AUDIOLINEINPUTCONTROLLER_HH_

#include "AudioInputController.hh"

/**
 * Class for handling operations between audio stream and out queue (pipe to
 * recognizer). Stores the whole audio data. Uses a specified audio input
 * stream (usually microphone) as the source.
 */
class AudioLineInputController  :  public AudioInputController
{
  
public:
  
  /** Constructs an audio input controller.
   * \param out_queue The out queue which the audio messages should be sent
   *                  to. */
  AudioLineInputController(msg::OutQueue *out_queue);

  /** Destructs the controller. */
  virtual ~AudioLineInputController();
  
  /** Pauses (or unpauses) audio input reading. Only in paused state is playback
   * allowed. Disconnects (or connects) input buffer from audio stream.
   * \param pause True for pause, false for unpause. */
  virtual void pause_listening(bool pause);

  /** \return Index of last audio sample read from audio input stream (this
   *          is the same as size of audio data. */
  virtual unsigned long get_audio_cursor() const;
  
  /** Resets cursors thus starting to send audio messages from the beginning. */
  virtual void reset_cursors();

protected:

  /** Reads audio samples from audio input stream. */
  virtual void read_input();

private:

  /** Audio data buffer for audio input stream. */
  AudioBuffer m_input_buffer;

};


#endif /*AUDIOLINEINPUTCONTROLLER_HH_*/
