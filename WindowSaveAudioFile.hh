
#ifndef WINDOWSAVEAUDIOFILE_HH_
#define WINDOWSAVEAUDIOFILE_HH_

#include "WindowSaveFile.hh"
#include "AudioInputController.hh"

class WindowSaveAudioFile  :  public WindowSaveFile
{

public:

  WindowSaveAudioFile(const PG_Widget *parent, AudioInputController *audio_input)
    : WindowSaveFile(parent, "Save Audio File"), m_audio_input(audio_input) { }
  
  virtual ~WindowSaveAudioFile() { }

protected:

  virtual bool write_file()
  {
    if (!audio::write_wav_data(this->get_filename(),
                               this->m_audio_input->get_audio_data(),
                               this->m_audio_input->get_audio_data_size()))
    {
      this->error("Could not write audio file.", ERROR_NORMAL);
      return false;
    }
    return true;
  }

private:

  AudioInputController *m_audio_input;
  
};

#endif /*WINDOWSAVEAUDIOFILE_HH_*/
