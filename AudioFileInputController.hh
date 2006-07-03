
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
  virtual bool start_listening();
  virtual void pause_listening(bool pause);

protected:

  virtual unsigned long read_input();

private:

  AudioOutputStream m_audio_output;
  unsigned long m_write_cursor;
};
//*/
#endif /*AUDIOFILEINPUTCONTROLLER_HH_*/
