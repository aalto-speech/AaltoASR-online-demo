
#include "WindowSaveFile.hh"

WindowSaveFile::WindowSaveFile(const PG_Widget *parent,
                               AudioInputController *audio_input)
  : WindowFile(parent, "Save Audio File")
{
  this->m_audio_input = audio_input;
}

bool
WindowSaveFile::do_ok()
{
  bool overwrite = true;
  
  if (this->file_exists())
    overwrite = this->confirm_overwrite();

  if (overwrite)
    return this->write_file();

  return false;
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

bool
WindowSaveFile::file_exists()
{
  FILE *file = fopen(this->get_filename().data(), "r");
  if (file != NULL) {
    fclose(file);
    return true;
  }
  return false;
}

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

