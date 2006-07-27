
#ifndef SCROLLBAR_HH_
#define SCROLLBAR_HH_
/*
// This has been copied and modified from PG_ScrollBar.h ! !

#include "pgthemewidget.h"
#include "pgbutton.h"

class ScrollBar : public PG_ThemeWidget {

protected:

#ifndef DOXYGEN_SKIP
class ScrollButton : public PG_Button {
  public:

    ScrollButton(ScrollBar* parent, const PG_Rect& r = PG_Rect::null);
    virtual ~ScrollButton();

    void SetTickMode(bool on);

  protected:

    bool eventMouseButtonDown(const SDL_MouseButtonEvent* button);
    bool eventMouseMotion(const SDL_MouseMotionEvent* motion);
    ScrollBar* GetParent();
    bool eventMouseButtonUp(const SDL_MouseButtonEvent* button);
    int GetPosFromPoint(PG_Point p);

  private:

    PG_Point offset;

    bool my_tickMode;

  };
#endif    // DOXYGEN_SKIP


public:

  //! ScrollbarType
  typedef enum {
    VERTICAL, //!< vertical scrollbar
    HORIZONTAL  //!< horizontal scrollbar
  } ScrollDirection;

  //! Widget ID's
  enum {
    IDSCROLLBAR_UP = PG_WIDGETID_INTERNAL + 1,    //!<ID Scrollbar Button "up"
    IDSCROLLBAR_DOWN = PG_WIDGETID_INTERNAL + 2,  //!< ID Scrollbar Button "down"
    IDSCROLLBAR_LEFT = PG_WIDGETID_INTERNAL + 3,    //!< ID Scrollbar Button "left"
    IDSCROLLBAR_RIGHT = PG_WIDGETID_INTERNAL + 4, //!< ID Scrollbar Button "right"
    IDSCROLLBAR_DRAG = PG_WIDGETID_INTERNAL + 5   //!< ID Scrollbar Button "drag"
  };

  template<class datatype> class SignalScrollPos : public PG_Signal2<ScrollBar*, datatype> {};
  template<class datatype> class SignalScrollTrack : public PG_Signal2<ScrollBar*, datatype> {};

  ScrollBar(PG_Widget* parent, const PG_Rect& r = PG_Rect::null, ScrollDirection direction = VERTICAL, int id = -1, const char* style="Scrollbar");

  virtual ~ScrollBar();
  void LoadThemeStyle(const char* widgettype);
  void SetPosition(Sint32 pos);
  Sint32 GetPosition();
  void SetRange(Sint32 min, Sint32 max);
  Sint32 GetMinRange();
  Sint32 GetMaxRange();
  void SetLineSize(Uint16 ls);
  void SetPageSize(Uint16 ps);

  SignalScrollPos<Sint32> sigScrollPos;
  SignalScrollTrack<Sint32> sigScrollTrack;
  
protected:

  void eventSizeWidget(Uint16 w, Uint16 h);

  bool eventMouseMotion(const SDL_MouseMotionEvent* motion);

  bool eventMouseButtonDown(const SDL_MouseButtonEvent* button);

  virtual bool handleButtonClick(PG_Button* button);

  bool eventMouseButtonUp(const SDL_MouseButtonEvent* button);

  Sint32 scroll_min;
  Sint32 scroll_max;
  Sint32 scroll_current;
  Uint16 my_linesize;
  Uint16 my_pagesize;

  PG_Button* scrollbutton[2];
  ScrollButton* dragbutton;
  PG_Rect position[4];

  ScrollDirection sb_direction;

  virtual void RecalcPositions();

  friend class ScrollButton;

private:
  
  ScrollBar(const ScrollBar&);
  ScrollBar& operator=(ScrollBar&);

};
//*/
#endif /*SCROLLBAR_HH_*/
