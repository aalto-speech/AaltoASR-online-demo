
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

  AudioFileInputController(const char *filename, msg::OutQueue *out_queue);
  virtual ~AudioFileInputController();
  
  virtual bool initialize();
  virtual void terminate();

  // Starts a new thread which listens audio in and sends it to out queue
  virtual bool start_listening();
//  virtual void stop_listening();
  virtual void pause_listening(bool pause);
  
//  unsigned long get_sample_rate() const;
//  unsigned int get_bytes_per_sample() const;
//  inline const std::string* get_audio_data() const { return &this->m_audio_data; };
  
protected:

//  virtual void do_listening();
  virtual unsigned long read_input();

private:

  AudioOutputStream m_audio_output;
  unsigned long m_write_cursor;
};
//*/
#endif /*AUDIOFILEINPUTCONTROLLER_HH_*/
