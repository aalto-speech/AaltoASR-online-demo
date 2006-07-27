
#ifndef AUDIOINPUTCONTROLLER_HH_
#define AUDIOINPUTCONTROLLER_HH_

#include <iostream>
#include <string.h>
#include <queue>
//#include "OutQueueController.hh"
#include "AudioStream.hh"
#include "msg.hh"
//#include "AudioOutputStream.hh"
//#include "ErrorHandler.hh"
//#include "FileOutput.hh"

class AudioInputController//  :  public ErrorHandler
{
  
public:
  
  AudioInputController(msg::OutQueue *out_queue);
  virtual ~AudioInputController();
  
  bool initialize();
  void terminate();

  unsigned long operate();
  virtual void pause_listening(bool pause);
  inline bool is_paused() const { return this->m_paused; }
  
  void play(unsigned long from, unsigned long length = 0);
  void stop();

  void reset();
  virtual void reset_cursors();

  /** m_audio_data can only be modified inside the class. */
  inline const AUDIO_FORMAT* get_audio_data() const { return (AUDIO_FORMAT*)this->m_audio_data.data(); }
  inline unsigned long get_audio_data_size() const { return this->m_audio_data.size() / sizeof(AUDIO_FORMAT); }

  /** Tells the size of data sent to the recognizer. */
  inline unsigned long get_read_cursor() const { return this->m_recognizer_cursor; }
  virtual unsigned long get_audio_cursor() const = 0;
  
protected:

  virtual void read_input() = 0;
  virtual bool open_stream(bool input_stream);
  
  std::string m_audio_data;
  msg::OutQueue *m_out_queue;
  AudioStream m_audio_stream;
  
private:

//  AudioOutputStream m_speakers;

  AudioBuffer m_output_buffer;
  /** read cursor in bytes! */
  unsigned long m_recognizer_cursor;

  bool m_playback;  
  unsigned long m_playback_from;
  unsigned long m_playback_length;
  unsigned long m_playback_played;

  bool m_paused;

};

#endif /*AUDIOINPUTCONTROLLER_HH_*/
