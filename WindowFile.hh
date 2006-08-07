
#ifndef WINDOWFILE_HH_
#define WINDOWFILE_HH_

#include <pglineedit.h>
#include "WindowChild.hh"

class WindowFile  :  public WindowChild
{
  
public:

  WindowFile(const PG_Widget *parent, const std::string &title);
  virtual ~WindowFile() { }
  
  virtual void initialize();
  
  inline const std::string get_filename() const;
  
protected:

  void focus_textbox();

  bool file_exists();
  virtual void do_opening();
    
private:

  PG_LineEdit *m_filename_textbox;

};

const std::string
WindowFile::get_filename() const
{
  return this->m_filename_textbox->GetText();
}

#endif /*WINDOWFILE_HH_*/
