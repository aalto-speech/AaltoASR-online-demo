
#ifndef WINDOWOPENFILE_HH_
#define WINDOWOPENFILE_HH_

#include "WindowFile.hh"
#include "AudioFileInputController.hh"

class WindowOpenFile  :  public WindowFile
{
  
public:

  WindowOpenFile(const PG_Widget *parent, AudioFileInputController *audio_input)
    : WindowFile(parent, "Open Audio File"), m_audio_input(audio_input) { }
  virtual ~WindowOpenFile() { }
  
protected:

  virtual bool do_ok()
  {
    // Try to load audio file.
    if (this->m_audio_input->load_file(this->get_filename())) {
      return true;
    }
    else {
      this->error("Could not open audio file.", ERROR_NORMAL);
      return false;
    }
  }

private:

  AudioFileInputController *m_audio_input;

};

#endif /*WINDOWOPENFILE_HH_*/
