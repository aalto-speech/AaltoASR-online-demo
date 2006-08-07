
#ifndef WINDOWOPENFILE_HH_
#define WINDOWOPENFILE_HH_

#include "WindowFile.hh"

class WindowOpenFile  :  public WindowFile
{
  
public:

  WindowOpenFile(const PG_Widget *parent, const std::string &title);
  virtual ~WindowOpenFile() { }
  
protected:

  virtual bool do_ok();
  virtual bool read_file() = 0;

};

#endif /*WINDOWOPENFILE_HH_*/
