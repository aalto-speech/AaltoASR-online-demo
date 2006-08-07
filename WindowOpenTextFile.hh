
#ifndef WINDOWOPENTEXTFILE_HH_
#define WINDOWOPENTEXTFILE_HH_

#include "WindowOpenFile.hh"
#include "str.hh"

class WindowOpenTextFile  :  public WindowOpenFile
{
  
public:

  WindowOpenTextFile(const PG_Widget *parent, std::string &content)
    : WindowOpenFile(parent, "Open Text File"), m_content(content) { }
    
  virtual ~WindowOpenTextFile() { }
  
protected:

  virtual bool read_file()
  {
    FILE *file = fopen(this->get_filename().data(), "r");
    if (file == NULL) {
      this->error("Could not open text file.", ERROR_NORMAL);
      this->focus_textbox();
      return false;
    }
      
    if (!str::read_file(&this->m_content, file)) {
      this->error("Could not read from text file.", ERROR_NORMAL);
      this->focus_textbox();
      return false;
    }
    return true;
  }

private:
  
  std::string &m_content;
  
};
#endif /*WINDOWOPENTEXTFILE_HH_*/
