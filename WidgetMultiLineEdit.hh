
#ifndef WIDGETMULTILINEEDIT_HH_
#define WIDGETMULTILINEEDIT_HH_

#include <pglineedit.h>
#include "WidgetScrollBar.hh"
#include <pgtheme.h>
#include <pgstring.h>

/** This class has been copied from pgmultilineedit.h and made a few changes
 * that were impossible to implement by deriving. */
class WidgetMultiLineEdit : public PG_LineEdit {
public:

  WidgetMultiLineEdit(PG_Widget* parent,
                      const PG_Rect& r,
                      const char* style="LineEdit",
                      int maximumLength = 1000000);
  // Change: (Virtual) destructor added.
  virtual ~WidgetMultiLineEdit() { }
  virtual void SetText(const char* new_text); 
  
  // Change: This function is moved to public.
  void SetVPosition(int line);
  // Change: This function is new.
  int GetVPosition() const;

protected:

  // Change: This function is new.
  virtual void eventInputFocusLost(PG_MessageObject *newfocus);

  void eventBlit(SDL_Surface* surface, const PG_Rect& src, const PG_Rect& dst);
  bool eventKeyDown(const SDL_KeyboardEvent* key);
  bool eventMouseButtonDown(const SDL_MouseButtonEvent* button);
  bool eventMouseMotion(const SDL_MouseMotionEvent* motion);
  bool eventMouseButtonUp(const SDL_MouseButtonEvent* button);
  virtual void InsertChar(const PG_Char* c);
  virtual void DeleteChar(Uint16 pos);
  bool handleScroll(PG_ScrollBar* widget, long data);

private:

  void FindWordRight();
  void FindWordLeft();
  void DeleteSelection();
  void GetCursorTextPosFromScreen(int x, int y, unsigned int& horzOffset, unsigned int& lineOffset);
  void GetCursorTextPos(unsigned int& horzOffset, unsigned int& lineOffset);
  void SetCursorTextPos(unsigned int offset, unsigned int line);
  int ConvertCursorPos(unsigned int offset, unsigned int line);
  void GetCursorPos(int& x, int& y);
  void SetCursorPos(int p);
  void DrawText(const PG_Rect& dst);
  void DrawTextCursor();
  void CreateTextVector(bool bSetupVScroll = true);
  void SetupVScroll();
  std::vector<PG_String> my_textdata;
  // Change: Use our own scroll bar because it's better.
//  PG_ScrollBar* my_vscroll;
  WidgetScrollBar* my_vscroll;
  int my_firstLine;
  int my_mark;
  bool my_isCursorAtEOL;
  bool my_allowHiddenCursor;  
};


#endif /*WIDGETMULTILINEEDIT_HH_*/
