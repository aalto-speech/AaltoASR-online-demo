
#ifndef WINDOWSAVETEXTFILE_HH_
#define WINDOWSAVETEXTFILE_HH_

#include "WindowSaveFile.hh"

/** A pop-up window to save a text file. */
class WindowSaveTextFile  :  public WindowSaveFile
{

public:

  WindowSaveTextFile(const PG_Widget *parent, const std::string &content)
    : WindowSaveFile(parent, "Save Text File"), m_content(content) { }
  
  virtual ~WindowSaveTextFile() { }

protected:

  virtual bool write_file()
  {
    FILE *file = fopen(this->get_filename().data(), "w");

    if (file == NULL) {
      this->error("Could not open file for writing.", ERROR_NORMAL);
      return false;
    }
    
    bool ret_val = true;
    if (fwrite(this->m_content.data(), sizeof(char), this->m_content.size(), file) != this->m_content.size()){//fprintf(file, "%s", this->m_content.data()) < 0) {
      this->error("Could not write text into file.", ERROR_NORMAL);
      ret_val = false;
    }
    
    fclose(file);
    return ret_val;
  }

private:

  std::string m_content;
  
};

#endif /*WINDOWSAVETEXTFILE_HH_*/
