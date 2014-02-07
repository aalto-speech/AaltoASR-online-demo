
#ifndef WINDOWOPENAUDIOFILE_HH_
#define WINDOWOPENAUDIOFILE_HH_

#include "WindowOpenFile.hh"
#include "AudioInputController.hh"

/** A pop-up window to load an audio file. */
class WindowOpenAudioFile  :  public WindowOpenFile
{
  
public:

  WindowOpenAudioFile(const PG_Widget *parent, AudioInputController *audio_input)
    : WindowOpenFile(parent, "Open Audio File"), m_audio_input(audio_input) { }
    
  virtual ~WindowOpenAudioFile() { }
  
protected:

  virtual bool read_file()
  {
    // Try to load audio file.
    if (!this->m_audio_input->load_file(this->get_filename())) {
      this->error("Could not open audio file.", ERROR_NORMAL);
      this->focus_textbox();
      return false;
    }
    return true;
  }

private:

  AudioInputController *m_audio_input;

};


#endif /*WINDOWOPENAUDIOFILE_HH_*/
