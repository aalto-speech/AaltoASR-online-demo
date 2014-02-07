
#include "WindowOpenFile.hh"

WindowOpenFile::WindowOpenFile(const PG_Widget *parent,
                               const std::string &title)
  : WindowFile(parent, title)
{
}

bool
WindowOpenFile::do_ok()
{
  if (!this->file_exists()) {
    this->error("File does not exist.", ERROR_NORMAL);
    this->focus_textbox();
    return false;
  }

  return this->read_file();
}
