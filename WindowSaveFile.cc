
#include "WindowSaveFile.hh"
#include "WindowMessageBox.hh"

WindowSaveFile::WindowSaveFile(const PG_Widget *parent,
                               const std::string &title)
  : WindowFile(parent, title)
{
}

bool
WindowSaveFile::do_ok()
{
  bool overwrite = true;
  bool ret_val = false;
  
  if (this->file_exists())
    overwrite = this->confirm_overwrite();

  if (overwrite)
    ret_val = this->write_file();

  if (!ret_val)
    this->focus_textbox();
    
  return ret_val;
}

bool
WindowSaveFile::confirm_overwrite()
{
  int ret_val;
  // Ask whether to overwrite file.
  WindowMessageBox window(this->m_window,
                          "Confirm overwrite",
                          "File exists. Do you want to overwrite?",
                          "Yes",
                          "No");
  window.initialize();
  ret_val = this->run_child_window(&window);
  
  return ret_val == 1;
}

/*
bool
WindowSaveFile::write_file()
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
//*/
