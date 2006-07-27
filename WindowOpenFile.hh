
#ifndef WINDOWOPENFILE_HH_
#define WINDOWOPENFILE_HH_

#include "WindowFile.hh"
#include "AudioFileInputController.hh"

class WindowOpenFile  :  public WindowFile
{
  
public:

  WindowOpenFile(const PG_Widget *parent, AudioFileInputController *audio_input);
  virtual ~WindowOpenFile() { }
  
protected:

  virtual bool do_ok();

private:

  AudioFileInputController *m_audio_input;

};

#endif /*WINDOWOPENFILE_HH_*/
