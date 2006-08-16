
#ifndef WINDOWTEXTEDIT_HH_
#define WINDOWTEXTEDIT_HH_

#include "WindowChild.hh"
#include "WidgetMultiLineEdit.hh"

class WindowTextEdit  :  public WindowChild
{
public:

  WindowTextEdit(const PG_Widget *parent,
                 const std::string &title,
                 Uint16 width,
                 Uint16 height,
                 std::string *text);

  virtual ~WindowTextEdit() { }
  
  virtual void initialize();

protected:

  virtual bool do_ok();
  
private:

  std::string *m_text;
  WidgetMultiLineEdit *m_text_area;

};

#endif /*WINDOWTEXTEDIT_HH_*/
