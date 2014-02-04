
/****************
 * These implementations are copied from pgmultilineedit.h. Some changes are
 * made to fix some bugs and also to change some behaviour.
 * "Change:" and "CHANGE:" tags help you find these changes.
 */

#include <pgapplication.h>
#include "WidgetMultiLineEdit.hh"
#include "WidgetScrollBar.hh"

using namespace std;

WidgetMultiLineEdit::WidgetMultiLineEdit(PG_Widget* parent, const PG_Rect& r, const char* style, int maximumLength) 
: PG_LineEdit(parent, r, style, maximumLength) { 
  // Change: Use our own scroll bar.
//  my_vscroll = new PG_ScrollBar(this, PG_Rect(r.w-16,0,16,r.h));
  my_vscroll = new WidgetScrollBar(this, PG_Rect(r.w-16,0,16,r.h));
  my_isCursorAtEOL = false; 
  my_allowHiddenCursor = false;
  my_firstLine = 0;   
  my_vscroll->sigScrollPos.connect(slot(*this, &WidgetMultiLineEdit::handleScroll));
  my_vscroll->sigScrollTrack.connect(slot(*this, &WidgetMultiLineEdit::handleScroll));
  my_vscroll->Hide();
  my_mark = -1;
}

/** CHANGE: A SMALL CHANGE MADE HERE. */
bool WidgetMultiLineEdit::handleScroll(PG_ScrollBar* widget, long data) {
  SetVPosition(my_vscroll->GetPosition());
  // Change: Update screen here.
  this->Update();
  my_allowHiddenCursor = true;
  return true;
}

void WidgetMultiLineEdit::SetVPosition(int line) {
  if (line < 0) {
    line = 0;
  }
  
  if (line > my_vscroll->GetMaxRange()) {
    line = my_vscroll->GetMaxRange();
  }
  
  my_firstLine = line;
  
  if (my_vscroll->GetPosition() != line) {
    my_vscroll->SetPosition(line);
  }

  Update();
}

/** CHANGE: NEW FUNCTION */
int
WidgetMultiLineEdit::GetVPosition() const
{
  return my_firstLine;
}

void WidgetMultiLineEdit::eventBlit(SDL_Surface* surface, const PG_Rect& src, const PG_Rect& dst) {
  PG_ThemeWidget::eventBlit(surface, src, dst);
  DrawText(dst);
}

void WidgetMultiLineEdit::DrawText(const PG_Rect& dst) {
  int _x = 3;
  int _y = 3; 

  // should we draw the cursor ? 
  if(IsCursorVisible()) { 
    DrawTextCursor(); 
  }

  // figure out the cursor position that we start at 
  int pos = 0; 
  for (unsigned int i = 0; i < (unsigned int)my_firstLine; ++i) { 
    pos += my_textdata[i].size(); 
  } 

  // draw text 
  int maxLines = my_height/GetFontSize() + 1;
  int endpos, start, end;

  int x1 = 0;
  Uint16 w = 0;
  int offset = 0;

  for (unsigned int i = my_firstLine; i < (unsigned int)my_firstLine + maxLines && i < my_textdata.size(); ++i) { 
    endpos = pos + my_textdata[i].size();
    start = (my_cursorPosition < my_mark ? my_cursorPosition : my_mark); 
    end  = (my_cursorPosition >= my_mark ? my_cursorPosition : my_mark); 
    
    // check if we are in the highlighted section 
    if (my_mark != -1 && my_mark != my_cursorPosition && pos <= end && endpos >= start) { 
      x1 = _x;
      offset = 0;
      
      // draw the initial unhighlighted part 
      if (pos < start) { 
        string s = my_textdata[i].substr(0, start-pos); 
        PG_Widget::DrawText(x1, _y, s.c_str()); 
        PG_FontEngine::GetTextSize(s.c_str(), GetFont(), &w); 
        x1 += w; 
        offset = start-pos; 
      }

      string middlepart = my_textdata[i].c_str() + offset;
      // check if the end part is unhighlighted
      if (endpos > end) {
        middlepart = middlepart.substr(0, middlepart.size() - (endpos-end));
        string s = my_textdata[i].substr(end - pos, my_textdata[i].size() - (end - pos));
        PG_FontEngine::GetTextSize(middlepart.c_str(), GetFont(), &w);
        PG_Widget::DrawText(x1+w, _y, s.c_str());
      }
      
      PG_Color color(GetFontColor()); 
      PG_Color inv_color(255 - color.r, 255 - color.g, 255 - color.b);      
      SetFontColor(inv_color);
      PG_FontEngine::GetTextSize(middlepart.c_str(), GetFont(), &w);
      SDL_Rect rect = {static_cast<Sint16>(x + x1),
                       static_cast<Sint16>(y + _y),
                       w,
                       static_cast<Uint16>(GetFontHeight())};
      SDL_Surface* screen = PG_Application::GetScreen();
      SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, color.r, color.g, color.b));
      PG_Widget::DrawText(x1, _y, middlepart.c_str());
      SetFontColor(color); 
    } 
    else { 
      PG_Widget::DrawText(_x, _y, my_textdata[i].c_str());
    } 
    _y += GetFontHeight(); 
    pos += my_textdata[i].size();
  } 
} 

void WidgetMultiLineEdit::DrawTextCursor() {
  int x = my_xpos + 1; 
  int y = my_ypos + 1; 
  int xpos, ypos;
  GetCursorPos(xpos, ypos); 

  // check for a hidden cursor 
  if(!my_allowHiddenCursor) { 
    // scroll up for cursor 
    while (ypos < 0 && my_firstLine > 0) { 
      SetVPosition(--my_firstLine); 
      GetCursorPos(xpos, ypos); 
    } 

    // scroll down for cursor 
    while (ypos + GetFontHeight() > my_height && my_firstLine < my_vscroll->GetMaxRange()) { 
      SetVPosition(++my_firstLine); 
      GetCursorPos(xpos, ypos); 
    } 
  } 

  // draw simple cursor 
  if(my_srfTextCursor == NULL) { 
    DrawVLine(xpos + 2, ypos + 2, GetFontHeight()-4, PG_Color()); 
  } 
  // draw a nice cursor bitmap 
  else { 
    PG_Rect src, dst; 
    PG_Rect rect(x + xpos, y + ypos + GetFontHeight()/2 - my_srfTextCursor->h/2, 
    my_srfTextCursor->w, my_srfTextCursor->h); 
    GetClipRects(src, dst, rect); 
    PG_Widget::eventBlit(my_srfTextCursor, src, dst); 
  } 
}

void WidgetMultiLineEdit::FindWordRight() { 
  unsigned int currentPos = my_cursorPosition; 
  
  // step off the initial space 
  ++currentPos; 
  
  // find the next space 
  while (currentPos-1 <= my_text.size() && my_text[currentPos-1] != ' ' && my_text[currentPos-1] != '\n') {
    ++currentPos; 
  }
  
  // go to the end of multiple spaces 
  while (currentPos <= my_text.size() && (my_text[currentPos] == ' ' || my_text[currentPos] == '\n')) {
    ++currentPos;
  }

  SetCursorPos(currentPos); 
}

void WidgetMultiLineEdit::FindWordLeft() { 
  unsigned int currentPos = my_cursorPosition; 

  // step off the initial space(s) 
  while (currentPos-1 >= 0 && (my_text[currentPos-1] == ' ' || my_text[currentPos-1] == '\n')) {
    --currentPos;
  }
  
  // find the next space 
  while (currentPos-1 >= 0 && my_text[currentPos-1] != ' ' && my_text[currentPos-1] != '\n') {
    --currentPos;
  }

  SetCursorPos(currentPos); 
}

void WidgetMultiLineEdit::GetCursorTextPosFromScreen(int x, int y, unsigned int& horzOffset, unsigned int& lineOffset) { 
  // check for an empty text box 
  if (my_textdata.size() == 0) { 
    horzOffset = 0;
    lineOffset = 0;
    return; 
  }

  // get the line number 
  int ypos = (y - my_ypos - 3) / GetFontHeight() + my_firstLine; 

  // stay within limits 
  if (ypos < 0) {
    ypos = 0;
  }
  
  if ((unsigned int)ypos >= my_textdata.size()) {
    ypos = my_textdata.size()-1;
  }

  unsigned int min = (unsigned int)-1; 
  unsigned int min_xpos = 0; 
  
  // loop through to find the closest x position 
  string temp;
  for (Uint16 i = 0; i <= my_textdata[ypos].size(); ++i) {  
    // get the string up to that point    
    temp = my_textdata[ypos].substr(0, i);
  
    // get the distance for that section 
    Uint16 w; 
    PG_FontEngine::GetTextSize(temp.c_str(), GetFont(), &w); 
    unsigned int dist = abs(x - (my_xpos + 3 + w)); 
    
    // update minimum 
    if (dist < min) { 
      min = dist; 
      min_xpos = i; 
    } 
  }

  // set return data 
  horzOffset = min_xpos;
  lineOffset = (unsigned int)ypos; 
}

void WidgetMultiLineEdit::GetCursorTextPos(unsigned int& horzOffset, unsigned int& lineOffset) { 
  // check for an empty text box 
  if (my_textdata.size() == 0) { 
    horzOffset = 0;
    lineOffset = 0;
    return; 
  }

  unsigned int currentPos = my_cursorPosition; 
  unsigned int line = 0; 

  // cycle through the lines, finding where our cursor lands 
  for (vector<PG_String>::iterator i = my_textdata.begin(); i != my_textdata.end(); ++i) { 
    if(currentPos < i->size() || (currentPos <= i->size() && my_isCursorAtEOL)) {
      break;
    }
    currentPos -= i->size(); 
    line++;
  } 

  // if we're too far, assume we're at the end of the string 
  if (line >= my_textdata.size()) {  
    line = my_textdata.size()-1; 
    currentPos = my_textdata[line].size();
  } 
  
  // if we're too far on this line, assum we're at the end of line
  if (currentPos > my_textdata[line].size()) {  
    currentPos = my_textdata[line].size(); 
  } 

  horzOffset = currentPos;
  lineOffset = line; 
} 

void WidgetMultiLineEdit::GetCursorPos(int& x, int& y) { 
  // check for an empty text box 
  if (my_textdata.size() == 0) { 
    x = 0;
    y = 0;
    return; 
  } 

  // get the cursor text position 
  unsigned int currentPos, line; 
  GetCursorTextPos(currentPos, line); 

  // now get the x,y position 
  string temp = my_textdata[line].substr(0, currentPos);

  Uint16 w; 
  PG_FontEngine::GetTextSize(temp.c_str(), GetFont(), &w); 
  
  x = w; 
  y = (line - my_firstLine)*GetFontHeight(); 
} 

void WidgetMultiLineEdit::CreateTextVector(bool bSetupVScroll) { 
  int w = my_width - 6 - ((my_vscroll->IsVisible() || !my_vscroll->IsHidden()) ? my_vscroll->w : 0); 

  // now split the text into lines 
  my_textdata.clear(); 
  unsigned int start = 0, end = 0, last = 0; 
  
  do { 
    Uint16 lineWidth = 0; 
    PG_String temp = my_text.substr(start, end-start);
    PG_FontEngine::GetTextSize(temp.c_str(), GetFont(), &lineWidth); 
    
    if (lineWidth > w) { 
      if (last == start) { 
        PG_String s = my_text.substr(start, end-start-1); 
        my_textdata.push_back(s); 
        start = --end; 
      } 
      else { 
        PG_String s = my_text.substr(start, last-start); 
        my_textdata.push_back(s); 
        start = last; 
        end = last-1; 
      } 
      last = start; 
    } 
    
    else if (my_text[end] == ' ') { 
      last = end+1; 
    } 
    
    else if (my_text[end] == '\n' || my_text[end] == '\0') { 
      PG_String s = my_text.substr(start, end-start+1); 
      my_textdata.push_back(s); 
      start = end+1; 
      last = start; 
    } 
  } while (end++ < my_text.size()); 

  // setup the scrollbar
  if(bSetupVScroll) {
    SetupVScroll(); 
  }
} 

/** CHANGE: FIXED ONE BUG HERE. */
void WidgetMultiLineEdit::SetupVScroll() {
  if (my_textdata.size()*GetFontHeight() < my_height) { 
    my_vscroll->SetRange(0, 0); 
    my_vscroll->Hide(); 
    SetVPosition(0);
    CreateTextVector(false);
  } 
  
  else {
    my_vscroll->SetRange(0, my_textdata.size() - my_height/GetFontHeight()); 
    if (my_firstLine > my_vscroll->GetMaxRange()) {
      SetVPosition(my_vscroll->GetMaxRange()); 
    }
  
    // Change: Original if statement causes eternal loops if text area is not
    // visible when setting text.
//    if (!my_vscroll->IsVisible() || my_vscroll->IsHidden()) { 
    if (my_vscroll->IsHidden()) { 
      // scrollbar makes the window less wide, so we have to redo the text 
      // (note: don't switch these next two lines, unless you like infinite loops) 
      my_vscroll->Show(); 
      // Change: Of course we have to setup the scroll bar again if we re-create
      // the text vector and thus allow the number of lines to change! (that is:
      // give parameter true)
      CreateTextVector(true);
//      CreateTextVector(false);
    } 
  } 
} 

/** CHANGE: THIS NEW FUNCTION FIXES THE INPUT FOCUS LOST BUG. */
void
WidgetMultiLineEdit::eventInputFocusLost(PG_MessageObject *newfocus)
{
  // This fixes the bug: now it doesn't highlight all the text from start
  // to cursor.
  int y = this->GetVPosition();
  int cursor = this->my_cursorPosition;
  this->my_mark = -1;
  
  PG_LineEdit::eventInputFocusLost(newfocus);
  this->my_cursorPosition = cursor;
  this->SetVPosition(y);
}


bool WidgetMultiLineEdit::eventKeyDown(const SDL_KeyboardEvent* key) {
  PG_Char c; 

  if(!IsCursorVisible()) {
    return false;
  }

  SDL_KeyboardEvent key_copy = *key; // copy key structure 
  PG_Application::TranslateNumpadKeys(&key_copy); 

  if ((key_copy.keysym.mod & KMOD_SHIFT) && my_mark == -1) {
    my_mark = my_cursorPosition; 
  }

  if(key_copy.keysym.mod & KMOD_CTRL) {
    switch(key_copy.keysym.sym) { 
      case SDLK_HOME: 
        if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
          my_mark = -1; 
        }
        SetCursorPos(0); 
        return true; 
      
      case SDLK_END: 
        if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
          my_mark = -1; 
        }
        SetCursorPos(my_text.length()); 
        return true; 
      
      case SDLK_RIGHT: 
        if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
          my_mark = -1; 
        }
        FindWordRight(); 
        return true; 
    
      case SDLK_LEFT: 
        if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
          my_mark = -1; 
        }
        FindWordLeft(); 
        return true; 
      
      case SDLK_UP: 
        my_allowHiddenCursor = true; 
        SetVPosition(--my_firstLine); 
        return true; 
    
      case SDLK_DOWN: 
        my_allowHiddenCursor = true; 
        SetVPosition(++my_firstLine); 
        return true;

      default:
        break;
    } 
  } 
  else if(key_copy.keysym.mod & (KMOD_ALT | KMOD_META)) { 
  } 
  else { 
    unsigned int currentPos, line; 
    int x, y; 
  
    switch(key_copy.keysym.sym) { 
      case SDLK_RIGHT: 
      case SDLK_LEFT: 
        if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
          my_mark = -1; 
        }
        // break here, we still want PG_LineEdit to handle these 
        break; 
      
      case SDLK_UP: 
        if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
        my_mark = -1; 
        }
        GetCursorPos(x, y); 
        GetCursorTextPosFromScreen(my_xpos + x + 3, my_ypos + y + 3 - GetFontHeight(), currentPos, line);
        SetCursorTextPos(currentPos, line); 
        return true; 
      
      case SDLK_DOWN: 
        if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
          my_mark = -1; 
        }
        GetCursorPos(x, y);
        GetCursorTextPosFromScreen(my_xpos + x + 3, my_ypos + y + 3 + GetFontHeight(), currentPos, line);
        SetCursorTextPos(currentPos, line); 
        return true; 
      
      case SDLK_PAGEUP: 
        if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
          my_mark = -1; 
        }       
        GetCursorPos(x, y); 
        GetCursorTextPosFromScreen(my_xpos + x + 3, my_ypos + y + 3 - (my_height - GetFontHeight()), currentPos, line); 
        SetCursorTextPos(currentPos, line); 
        return true; 
    
      case SDLK_PAGEDOWN: 
        if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
          my_mark = -1; 
        }
        
        GetCursorPos(x, y); 
        GetCursorTextPosFromScreen(my_xpos + x + 3, my_ypos + y + 3 + (my_height - GetFontHeight()), currentPos, line); 
        SetCursorTextPos(currentPos, line); 
        return true; 
      
      case SDLK_HOME: 
        if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
          my_mark = -1; 
        }
        GetCursorTextPos(currentPos, line); 
        SetCursorTextPos(0, line); 
        return true; 
    
      case SDLK_END: {
        if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
          my_mark = -1; 
        }
        GetCursorTextPos(currentPos, line); 
        int cursorPos = my_textdata[line].size() - (my_textdata[line][my_textdata[line].size()-1] == '\n' ? 1 : 0); 
        SetCursorTextPos(cursorPos, line);
      }
        return true; 

      case SDLK_RETURN: 
        InsertChar('\n'); 
        SetCursorPos(my_cursorPosition); 
        return true;
        
      default:
        break;
    } 
  } 

    SetCursorPos(my_cursorPosition); 
  
  return PG_LineEdit::eventKeyDown(key); 
}

/** CHANGE: A SMALL CHANGE MADE HERE. */
bool WidgetMultiLineEdit::eventMouseButtonDown(const SDL_MouseButtonEvent* button) { 
    
  // check for mousewheel 
  if ((button->button == 4 || button->button == 5) && my_vscroll->IsVisible()) { 
    // Change: This fixes the mouse wheel bug: if not scrolled between cursor
    // positioning and this moment, the mouse wheel wouldn't let hide the
    // cursor. Force to allow hidden cursor fixes the bug.
    my_allowHiddenCursor = true;
    
    if (button->button == 4) {
      SetVPosition(--my_firstLine); 
    }
    else {
      SetVPosition(++my_firstLine);
    }
    return true; 
  } 

  if (!GetEditable()) {
    return false; 
  }

  if (!IsCursorVisible()) {
    EditBegin(); 
  }

  // if we're clicking the scrollbar.... 
  if (my_vscroll->IsVisible() && button->x > my_xpos + my_width - my_vscroll->w) {
    return false; 
  }

  if (button->button == 1) { 
    Uint8* keys = SDL_GetKeyState(NULL); 
    
    if (!(keys[SDLK_LSHIFT] || keys[SDLK_RSHIFT])) {
      my_mark = -1;
    }
    
    unsigned int currentPos, line; 
    GetCursorTextPosFromScreen(button->x, button->y, currentPos, line); 
    SetCursorTextPos(currentPos, line); 
    if (!(keys[SDLK_LSHIFT] || keys[SDLK_RSHIFT])) {
      my_mark = my_cursorPosition;
    }
  } 
  
  return true; 
} 

bool WidgetMultiLineEdit::eventMouseMotion(const SDL_MouseMotionEvent* motion) {
  if (motion->state & SDL_BUTTON(1)) { 
      unsigned int currentPos, line; 
    GetCursorTextPosFromScreen(motion->x, motion->y, currentPos, line); 
    SetCursorTextPos(currentPos, line); 
  }
  
  return PG_LineEdit::eventMouseMotion(motion); 
} 

bool WidgetMultiLineEdit::eventMouseButtonUp(const SDL_MouseButtonEvent* button) {
  if(!GetEditable()) {
    return false;
  }

  if(!IsCursorVisible()) {
    EditBegin();
  }

  return true;
}

void WidgetMultiLineEdit::SetCursorTextPos(unsigned int offset, unsigned int line) { 
  my_allowHiddenCursor = false; 
  if (line < 0) {
    SetCursorPos(0); 
  }
  else if (line >= my_textdata.size()) { 
    SetCursorPos(my_text.size()); 
    my_isCursorAtEOL = false; 
  } 
  else { 
    PG_LineEdit::SetCursorPos(ConvertCursorPos(offset, line)); 
    my_isCursorAtEOL = (offset == my_textdata[line].size() && my_textdata[line].size() != 0);
    Update();
  } 
} 

int WidgetMultiLineEdit::ConvertCursorPos(unsigned int offset, unsigned int line) {
  unsigned int charCount = 0; 
  for (unsigned int i = 0; i < line; ++i) {
    charCount += my_textdata[i].size();
  }
  
  return charCount+offset;
} 

void WidgetMultiLineEdit::SetCursorPos(int p) {
  my_isCursorAtEOL = false; 
  my_allowHiddenCursor = false; 
  PG_LineEdit::SetCursorPos(p);
  //Update();
} 

void WidgetMultiLineEdit::InsertChar(const PG_Char& c) { 
  my_allowHiddenCursor = false; 
  if (my_mark != -1 && my_mark != my_cursorPosition) {
    DeleteSelection(); 
  }
  
  PG_LineEdit::InsertChar(c); 
  my_mark = -1; 
  CreateTextVector();
  Update();
} 

void WidgetMultiLineEdit::DeleteChar(Uint16 pos) { 
  my_allowHiddenCursor = false; 
  if (my_mark != -1 && my_mark != my_cursorPosition) { 
    Uint16 oldpos = my_cursorPosition; 
    DeleteSelection(); 
    // check if backspace was pressed 
    if (pos == oldpos-1) {
      my_cursorPosition++; 
    }
  } 
  else {
    PG_LineEdit::DeleteChar(pos);
  }
  
  my_mark = -1; 
  CreateTextVector();
  Update();
} 

void WidgetMultiLineEdit::DeleteSelection() {
  if (my_mark != -1 && my_mark != my_cursorPosition) {
    int start = (my_cursorPosition < my_mark ? my_cursorPosition : my_mark);
    int end  = (my_cursorPosition >= my_mark ? my_cursorPosition : my_mark);
    my_text.erase(start, end-start);
    if (my_mark < my_cursorPosition) {
      SetCursorPos(my_mark);
    }
    my_mark = -1;
  }
}

void WidgetMultiLineEdit::SetText(const char* new_text) { 
  PG_LineEdit::SetText(new_text);
  CreateTextVector();
  my_isCursorAtEOL = false;
  my_allowHiddenCursor = false;
  my_mark = -1;
//  SetVPosition(0);
}
