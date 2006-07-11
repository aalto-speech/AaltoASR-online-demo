
#ifndef WINDOWOPENFILE_HH_
#define WINDOWOPENFILE_HH_

#include "WindowFile.hh"
#include "AudioFileInputController.hh"

class WindowOpenFile  :  public WindowFile
{
  
public:

  WindowOpenFile(const PG_Widget *parent, AudioFileInputController *audio_input)
    : WindowFile(parent), m_audio_input(audio_input) { }
  
protected:

  virtual std::string get_title() const { return "Open Audio File"; }

  virtual void handle_ok_pressed()
  {
    // Try to load audio file.
    if (this->m_audio_input->load_file(this->get_filename())) {
      this->close(1);
    }
    else {
      this->error("Could not open audio file.", ERROR_NORMAL);
    }
  }

private:

  AudioFileInputController *m_audio_input;

};

#endif /*WINDOWOPENFILE_HH_*/
