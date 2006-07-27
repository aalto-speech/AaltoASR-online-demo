
#include "WindowOpenFile.hh"

WindowOpenFile::WindowOpenFile(const PG_Widget *parent,
                               AudioFileInputController *audio_input)
  : WindowFile(parent, "Open Audio File")
{
  this->m_audio_input = audio_input;
}

bool
WindowOpenFile::do_ok()
{
  if (!this->file_exists()) {
    this->error("File does not exist.", ERROR_NORMAL);
    this->focus_textbox();
    return false;
  }

  // Try to load audio file.
  if (!this->m_audio_input->load_file(this->get_filename())) {
    this->error("Could not open audio file.", ERROR_NORMAL);
    this->focus_textbox();
    return false;
  }

  return true;
}
