
#ifndef WINDOWFILE_HH_
#define WINDOWFILE_HH_

#include <pgbutton.h>
#include <pglineedit.h>
#include <pglabel.h>
#include "Window.hh"
//#include "AudioFileInputController.hh"

class WindowFile  :  public Window
{
  
public:

  WindowFile(const PG_Widget *parent);
  virtual ~WindowFile();

  virtual void initialize();
  
  inline const std::string get_filename() const { return this->m_filename_textbox->GetText(); }
  
protected:

  virtual std::string get_title() const = 0;

//  virtual void do_opening();
  virtual void do_running();
  virtual void do_closing(int return_value);

  bool handle_button(PG_Button *button);
  bool handle_close();

  virtual void handle_ok_pressed() = 0;
  
  virtual PG_Widget* create_window();
    
private:

  static std::string last_file;

  const PG_Widget *m_parent;
  PG_Label *m_text_label;
  PG_LineEdit *m_filename_textbox;
  PG_Button *m_ok_button;
  PG_Button *m_cancel_button;
  bool m_ok_pressed;
  
};

#endif /*WINDOWFILE_HH_*/
