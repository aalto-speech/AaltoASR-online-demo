/*
 * THIS HAS BEEN COPIED AND MODIFIED FROM PG_SCROLLBAR.CPP !!!!
*/

/*
#include "ScrollBar.hh"
#include <pgapplication.h>

ScrollBar::ScrollBar(PG_Widget* parent, const PG_Rect& r, ScrollDirection direction, int id, const char* style)
  : PG_ThemeWidget(parent, r, style)
{
  sb_direction = direction;

  SetID(id);

  scroll_min = 0;
  scroll_max = 4;
  scroll_current = 0;

  my_linesize = 1;
  my_pagesize = 5;

  scrollbutton[0] = new PG_Button(this);
  scrollbutton[0]->SetID((direction == VERTICAL) ? IDSCROLLBAR_UP : IDSCROLLBAR_LEFT);
  scrollbutton[0]->sigClick.connect(slot(*this, &ScrollBar::handleButtonClick));
  
  scrollbutton[1] = new PG_Button(this);
  scrollbutton[1]->SetID((direction == VERTICAL) ? IDSCROLLBAR_DOWN : IDSCROLLBAR_RIGHT);
  scrollbutton[1]->sigClick.connect(slot(*this, &ScrollBar::handleButtonClick));

  dragbutton = new ScrollButton(this);
  dragbutton->SetID(IDSCROLLBAR_DRAG);
  dragbutton->sigClick.connect(slot(*this, &ScrollBar::handleButtonClick));

  if(strcmp(style, "Scrollbar") != 0) {
    LoadThemeStyle("Scrollbar");
  }
  LoadThemeStyle(style);
  SetPosition(0);
}

ScrollBar::~ScrollBar() {}

void ScrollBar::LoadThemeStyle(const char* widgettype) {

  PG_ThemeWidget::LoadThemeStyle(widgettype, "Scrollbar");

  if(sb_direction == VERTICAL) {
    scrollbutton[0]->LoadThemeStyle(widgettype, "ScrollbarUp");
    scrollbutton[1]->LoadThemeStyle(widgettype, "ScrollbarDown");
  } else {
    scrollbutton[0]->LoadThemeStyle(widgettype, "ScrollbarLeft");
    scrollbutton[1]->LoadThemeStyle(widgettype, "ScrollbarRight");
  }

  dragbutton->LoadThemeStyle(widgettype, "ScrollbarDrag");

  if(sb_direction == VERTICAL) {
    dragbutton->LoadThemeStyle(widgettype, "ScrollbarDragV");
    PG_ThemeWidget::LoadThemeStyle(widgettype, "ScrollbarV");
  } else {
    dragbutton->LoadThemeStyle(widgettype, "ScrollbarDragH");
    PG_ThemeWidget::LoadThemeStyle(widgettype, "ScrollbarH");
  }
  RecalcPositions();
}

void ScrollBar::RecalcPositions() {
  
  if(sb_direction == VERTICAL) {
    position[0].x = 0;
    position[0].y = 0;
    position[0].w = w;
    position[0].h = w;

    position[1].x = 0;
    position[1].y = abs(h-w);
    position[1].w = w;
    position[1].h = w;

    position[2].x = 0;
    position[2].y = w;
    position[2].w = w;
    position[2].h = abs(h-(w*2));

    position[3].x = 0;
    position[3].w = w;
    position[3].h = (position[2].h / 2); //(scroll_max - scroll_min)) * scroll_windowsize;

    if((scroll_max - scroll_min) == 0) {
      position[3].y = position[2].y;
    } else {
      position[3].y = ((position[2].h - position[3].h) / (scroll_max - scroll_min)) * scroll_current;
    }
  } else {
    position[0].x = 0;
    position[0].y = 0;
    position[0].w = h;
    position[0].h = h;

    position[1].x = abs(w - h);
    position[1].y = 0;
    position[1].w = h;
    position[1].h = h;

    position[2].x = h;
    position[2].y = 0;
    position[2].w = abs(w-(h*2));
    position[2].h = h;

    position[3].y = 0;
    position[3].w = (Uint16)((double)position[2].w / 2.0);
    position[3].h = h;

    if((scroll_max - scroll_min) == 0) {
      position[3].x = position[2].x;
    } else {
      position[3].x = ((position[2].w - position[3].w) / (scroll_max - scroll_min)) * scroll_current;
    }
  }

  Sint32 pos =   scroll_current - scroll_min;

  if(sb_direction == VERTICAL) {
    position[3].x = 0;
    position[3].h = (Uint16)((double)position[2].h / ((double)position[2].h / (double)position[3].h));
    position[3].y = (Uint16)(position[0].h + (((double)position[2].h - (double)position[3].h) / (double)(scroll_max - scroll_min)) * (double)pos);
  } else {
    position[3].y = 0;
    position[3].w = (Uint16)((double)position[2].w / ((double)position[2].w / (double)position[3].w) );
    position[3].x = (Uint16)(position[0].w + (((double)position[2].w - (double)position[3].w) / (double)(scroll_max - scroll_min)) * (double)pos);
  }

  // bordersize
  for(int i=0; i<4; i++) {
    if(i == 3 || i == 2) {
      if(sb_direction == VERTICAL) {
        position[i].x += my_bordersize;
        if(position[i].w > 2*my_bordersize) {
          position[i].w -= 2*my_bordersize;
        }
      }
      else {
        position[i].y += my_bordersize;
        if(position[i].h > 2*my_bordersize) {
          position[i].h -= 2*my_bordersize;
        }
      }
      continue;
    }
    position[i].x += my_bordersize;
    position[i].y += my_bordersize;
    if(position[i].w > 2*my_bordersize) {
      position[i].w -= 2*my_bordersize;
    }
    if(position[i].h > 2*my_bordersize) {
      position[i].h -= 2*my_bordersize;
    }
  }
  if(scrollbutton[0] != NULL) {
    scrollbutton[0]->MoveWidget(position[0]);
  }
  if(scrollbutton[1] != NULL) {
    scrollbutton[1]->MoveWidget(position[1]);
  }
  dragbutton->MoveWidget(position[3]);
}

void ScrollBar::eventSizeWidget(Uint16 w, Uint16 h) {

  PG_ThemeWidget::eventSizeWidget(w, h);

  RecalcPositions();
  SetPosition(scroll_current);
  return;
}

bool ScrollBar::eventMouseMotion(const SDL_MouseMotionEvent* motion) {
  return PG_ThemeWidget::eventMouseMotion(motion);
}

bool ScrollBar::eventMouseButtonUp(const SDL_MouseButtonEvent* button) {
  int x;
  int y;
  int mx = dragbutton->my_xpos + dragbutton->my_width / 2;
  int my = dragbutton->my_ypos + dragbutton->my_height / 2;

  SDL_GetMouseState(&x, &y);

  switch (button->button) {
    case 1:
      if(sb_direction == VERTICAL) {
        if(y < my) {
          SetPosition(scroll_current - my_pagesize);
        } else {
          SetPosition(scroll_current + my_pagesize);
        }
      } else {
        if(x < mx) {
          SetPosition(scroll_current - my_pagesize);
        } else {
          SetPosition(scroll_current + my_pagesize);
        }
      }

      sigScrollPos(this, scroll_current);
      return true;

    case 4:
      if(scroll_current <= scroll_min + my_linesize) {
        SetPosition(scroll_min);
      } else {
        SetPosition(scroll_current - my_linesize);
      }
      sigScrollPos(this, scroll_current);
      return true;

    case 5:
      SetPosition(scroll_current + my_linesize);
      sigScrollPos(this, scroll_current);
      return true;
  }

  return PG_ThemeWidget::eventMouseButtonUp(button);
}


ScrollBar::ScrollButton::ScrollButton(ScrollBar* parent, const PG_Rect& r) : PG_Button(parent, r) {
  SetID(IDSCROLLBAR_DRAG);
  my_tickMode = false;
}

ScrollBar::ScrollButton::~ScrollButton() {}

bool ScrollBar::ScrollButton::eventMouseMotion(const SDL_MouseMotionEvent* motion) {
  PG_Point p;

  if(GetPressed()) {

    //SDL_GetMouseState(&x, &y);
    p = GetParent()->ScreenToClient(motion->x, motion->y);

    if(GetParent()->sb_direction == VERTICAL) {
      p.y -= offset.y;

      if(p.y < GetParent()->position[2].y) {
        p.y = GetParent()->position[2].y;
      }

      int maxy;
      if(!my_tickMode) {
        maxy = GetParent()->position[2].y + (GetParent()->position[2].h) - my_height;
      } else {
        maxy = GetParent()->my_height - my_height;
      }

      if(p.y > maxy) {
        p.y = maxy;
      }

      MoveWidget(GetParent()->position[2].x, p.y);
    } else {
      p.x -= offset.x;

      if(p.x < GetParent()->position[2].x) {
        p.x = GetParent()->position[2].x;
      }

      int maxx;
      if(!my_tickMode) {
        maxx = GetParent()->position[2].x + (GetParent()->position[2].w) - (my_width);
      } else {
        maxx = GetParent()->my_width - my_width;
      }

      if(p.x > maxx) {
        p.x = maxx;
      }

      MoveWidget(p.x, GetParent()->position[2].y);
    }

    int pos = GetPosFromPoint(p);
    if(GetParent()->scroll_current != pos || my_tickMode) {
      GetParent()->scroll_current = pos;
      GetParent()->sigScrollTrack(GetParent(), pos);
    }

  }

  return true;
}

bool ScrollBar::ScrollButton::eventMouseButtonDown(const SDL_MouseButtonEvent* button) {
  int x,y;

  if(button->button == 1) {
    SDL_GetMouseState(&x, &y);
    offset = ScreenToClient(x, y);
  }

  return PG_Button::eventMouseButtonDown(button);
}

ScrollBar* ScrollBar::ScrollButton::GetParent() {
  return (ScrollBar*)PG_Button::GetParent();
}

void ScrollBar::SetPosition(Sint32 pos) {

  if(pos < scroll_min) {
    pos = scroll_min;
  }

  if(pos > scroll_max) {
    pos = scroll_max;
  }

  scroll_current = pos;
  pos -= scroll_min;

  // check if we are currently in a drag operation
  if(dragbutton->GetPressed()) {
    return;
  }

  RecalcPositions();
}

int ScrollBar::GetPosition() {
  return scroll_current;
}

bool ScrollBar::handleButtonClick(PG_Button* button) {

  if(button == scrollbutton[0]) {   // UP | LEFT
    if(scroll_current == scroll_min) {
      return false;
    }
    SetPosition(scroll_current - my_linesize);
    sigScrollPos(this, scroll_current);
    return true;
  }

  if(button == scrollbutton[1]) {   // DOWN | RIGHT
    if(scroll_current == scroll_max) {
      return false;
    }
    SetPosition(scroll_current + my_linesize);
    sigScrollPos(this, scroll_current);
    return true;

  }

  return false;
}

bool ScrollBar::eventMouseButtonDown(const SDL_MouseButtonEvent* button) {
  return false;
}

bool ScrollBar::ScrollButton::eventMouseButtonUp(const SDL_MouseButtonEvent* button) {
  int pos = GetParent()->scroll_current; //my_tempPos;

  if(button->button != 1) {
    return false;
  }

  PG_Button::eventMouseButtonUp(button);

  //GetParent()->SetPosition(pos);
  GetParent()->sigScrollPos(GetParent(), pos);

  return true;
}

void ScrollBar::ScrollButton::SetTickMode(bool on) {
  my_tickMode = on;
}

void ScrollBar::SetRange(Sint32 min, Sint32 max) {
  scroll_min = min;
  scroll_max = max;
  if (scroll_current < scroll_min) {
    SetPosition(scroll_min);
  }
  if (scroll_current > scroll_max) {
    SetPosition(scroll_max);
  }
}

Sint32 ScrollBar::GetMinRange() {
  return scroll_min;
}

Sint32 ScrollBar::GetMaxRange() {
  return scroll_max;
}

int ScrollBar::ScrollButton::GetPosFromPoint(PG_Point p) {
  Sint32 range = (GetParent()->scroll_max - GetParent()->scroll_min);
  Sint32 pos = 0;

  if(p.x < 0)
    p.x = 0;
  if(p.y < 0)
    p.y = 0;

  //if(!my_tickMode) {
    if(GetParent()->sb_direction == VERTICAL) {
      pos = (int)((double)(p.y - GetParent()->position[2].y) / (((double)GetParent()->position[2].h - (double)GetParent()->position[3].h) / (double)range));
    } else {
      pos = (int)((double)(p.x - GetParent()->position[2].x) / (((double)GetParent()->position[2].w - (double)GetParent()->position[3].w) / (double)range));
    }

  if(pos < 0)
    pos = 0;

  pos = GetParent()->scroll_min+pos;

  if(pos > GetParent()->scroll_max) {
    pos = GetParent()->scroll_max;
  }

  if(pos < GetParent()->scroll_min) {
    pos = GetParent()->scroll_min;
  }

  return pos;
}

void ScrollBar::SetLineSize(Uint16 ls) {
  my_linesize = ls;
}

void ScrollBar::SetPageSize(Uint16 ps) {
  my_pagesize = ps;
}
//*/
