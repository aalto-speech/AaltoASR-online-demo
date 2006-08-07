
#ifndef WINDOWSAVEFILE_HH_
#define WINDOWSAVEFILE_HH_

#include "WindowFile.hh"

class WindowSaveFile  :  public WindowFile
{

public:

  WindowSaveFile(const PG_Widget *parent, const std::string &title);
  virtual ~WindowSaveFile() { }

protected:

  virtual bool do_ok();
  
private:

  bool confirm_overwrite();
  virtual bool write_file() = 0;
  
};

#endif /*WINDOWSAVEFILE_HH_*/
