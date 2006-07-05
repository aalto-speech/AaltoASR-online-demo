
#ifndef AUDIOFILEINPUTCONTROLLER_HH_
#define AUDIOFILEINPUTCONTROLLER_HH_

#include <sndfile.h>
#include "msg.hh"
#include "AudioOutputStream.hh"
#include "AudioInputController.hh"
//*
class AudioFileInputController  :  public AudioInputController
{
  
public:

  AudioFileInputController(OutQueueController *out_queue);
  virtual ~AudioFileInputController();
  
  virtual bool initialize();
  virtual void terminate();

  /** No need for locking. */
  bool load_file(const char *filename);
  
  void reset();

  // Starts a new thread which listens audio in and sends it to out queue
//  virtual bool start_listening();
  virtual void pause_listening(bool pause);
  
  inline bool is_eof() const
  {
    return this->get_read_cursor() >= this->get_audio_data_size();
  }

protected:

  virtual unsigned long read_input();
  
//  void check_eof() { this->m_eof = this->get_read_cursor() >= this->get_audio_data_size(); }

private:

  AudioOutputStream m_audio_output;
  unsigned long m_output_cursor;
//  bool m_sent_eof;
//  bool m_eof;
};
//*/
#endif /*AUDIOFILEINPUTCONTROLLER_HH_*/
