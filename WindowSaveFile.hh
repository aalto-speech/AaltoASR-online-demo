
#ifndef WINDOWSAVEFILE_HH_
#define WINDOWSAVEFILE_HH_

#include "WindowFile.hh"
#include "AudioInputController.hh"
#include "WindowMessageBox.hh"

class WindowSaveFile  :  public WindowFile
{

public:

  WindowSaveFile(const PG_Widget *parent, AudioInputController *audio_input)
    : WindowFile(parent, "Save Audio File"), m_audio_input(audio_input) { }

protected:

//  virtual std::string get_title() const { return "Save Audio File"; }

  virtual bool do_ok()
  {
    FILE *file = fopen(this->get_filename().data(), "r");
    bool overwrite = true;
    if (file != NULL) {
      fclose(file);

      // Ask whether to overwrite file.
      WindowMessageBox messagebox(this->m_window,
                                  "Confirm overwrite",
                                  "File exists. Do you want to overwrite?",
                                  "Yes",
                                  "No");
      messagebox.initialize();
      overwrite = (this->run_child_window(&messagebox) == 1); 
    }
    if (overwrite) {
      if (audio::write_wav_data(this->get_filename(),
                                this->m_audio_input->get_audio_data(),
                                this->m_audio_input->get_audio_data_size()))
      {
        // Audio saving successful.
        return true;
      }
      else {
        this->error("Could not write audio file.", ERROR_NORMAL);
      }
    }
    return false;
  }
  
private:

  AudioInputController *m_audio_input;
  
};

#endif /*WINDOWSAVEFILE_HH_*/
