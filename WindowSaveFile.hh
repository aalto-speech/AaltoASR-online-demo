
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

  virtual bool do_ok()
  {
    bool overwrite = true;
    
    if (this->file_exists())
      overwrite = this->confirm_overwrite();

    if (overwrite)
      return this->write_file();

    return false;
  }
  
/*
  virtual void handle_close_child_window(Window *child_window, int ret_val)
  {
    Window::handle_close_child_window(child_window, ret_val);
    
    if (child_window == this->m_overwrite_window) {
      if (ret_val == 1) {
        if (this->write_file()) {
          this->end_running(1);
        }
      }
      delete this->m_overwrite_window;
    }
  }
//*/
private:

  bool confirm_overwrite()
  {
    // Ask whether to overwrite file.
    WindowMessageBox window(this->m_window,
                            "Confirm overwrite",
                            "File exists. Do you want to overwrite?",
                            "Yes",
                            "No");
    window.initialize();
    this->run_child_window(&window);
  }

  bool file_exists()
  {
    FILE *file = fopen(this->get_filename().data(), "r");
    if (file != NULL) {
      fclose(file);
      return true;
    }
    return false;
  }

  bool write_file()
  {
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
    return false;
  }

  AudioInputController *m_audio_input;
//  Window *m_overwrite_window;
  
};

#endif /*WINDOWSAVEFILE_HH_*/
