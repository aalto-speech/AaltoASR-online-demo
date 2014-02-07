
#ifndef WINDOWTEXTEDIT_HH_
#define WINDOWTEXTEDIT_HH_

#include "WindowChild.hh"
#include "WidgetMultiLineEdit.hh"

/** A simple pop-up window to edit text string. */
class WindowTextEdit  :  public WindowChild
{
public:

  /**
   * \param parent Parent widget.
   * \param title Title of the window.
   * \param width Width of the window.
   * \param height Height of the window.
   * \param text Text string to edit. */
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
