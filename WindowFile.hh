
#ifndef WINDOWFILE_HH_
#define WINDOWFILE_HH_

#include "undefine.hh"
#include <pglineedit.h>
#include <pglabel.h>
#include "WindowChild.hh"

class WindowFile  :  public WindowChild
{
  
public:

  WindowFile(const PG_Widget *parent, const std::string &title);
  virtual ~WindowFile() { }
  
  virtual void initialize();
  
  inline const std::string get_filename() const { return this->m_filename_textbox->GetText(); }
  
protected:

  void focus_textbox();

  bool file_exists();
  virtual void do_opening();
  virtual void do_closing(int return_value);
    
private:

  static std::string last_file;

  PG_LineEdit *m_filename_textbox;

};

#endif /*WINDOWFILE_HH_*/
