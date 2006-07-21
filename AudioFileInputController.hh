
#ifndef AUDIOFILEINPUTCONTROLLER_HH_
#define AUDIOFILEINPUTCONTROLLER_HH_

#include <sndfile.h>
#include "msg.hh"
#include "AudioInputController.hh"
//*
class AudioFileInputController  :  public AudioInputController
{
  
public:

  AudioFileInputController(msg::OutQueue *out_queue);
  virtual ~AudioFileInputController();
  
//  virtual bool initialize();
//  virtual void terminate();

  /** No need for locking. */
  bool load_file(const std::string &filename);
  
  virtual void reset_cursors();

  virtual unsigned long get_audio_cursor() const { return this->m_output_buffer.get_frames_read(); }

  // Starts a new thread which listens audio in and sends it to out queue
  virtual void pause_listening(bool pause);
  
  inline bool is_eof() const
  {
    return this->get_read_cursor() >= this->get_audio_data_size();
  }

protected:

  virtual void read_input();
  
private:

//  AudioOutputStream m_audio_output;
  AudioBuffer m_output_buffer;
  unsigned long m_output_cursor;
};
//*/
#endif /*AUDIOFILEINPUTCONTROLLER_HH_*/
