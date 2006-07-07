
#ifndef WINDOW_HH_
#define WINDOW_HH_

#include <pgapplication.h>
#include <pgwindow.h>
#include <pgmessagebox.h>


class Window  :  public SigC::Object
{

public:

  Window();
  virtual ~Window();

  /** Call this once before using the window. */
  virtual void initialize();
  /** Call this every time before run. */  
  void open();
  /** This function returns when the window closes. 
   * \return Zero means quitting, other values are window specific. */
  int run();
  /** Request run function to end ASAP. 
   * \param return_value Defines the value that run function returns. You
   * should not use zero value because it is reserved for quitting purposes!
   * Value (1) is used for default closing (e.g. an error occurs and
   * window has to be closed). The return value is used only to give
   * information what window should be opened next. */
  void close(int return_value = 1);
  /** Request run function to end ASAP and return quitting signal. */
  void quit();
  
protected:

  enum ErrorType { ERROR_FATAL, ERROR_CLOSE, ERROR_NORMAL };

  void error(const std::string &message, ErrorType type);

  virtual void do_opening() { };
  virtual void do_running() = 0;
  virtual void do_closing(int return_value) { };
  
  inline virtual PG_Widget* create_window() { return new PG_Widget(NULL, PG_Rect(0, 0, PG_Application::GetScreenWidth(), PG_Application::GetScreenHeight()), false); }
  /** Opens and runs child window. Disables this window for that time.
   * Function returns after child window's run function returns. */
  virtual int run_child_window(Window *child_window);
  
  PG_Widget *m_window;
  
private:

  void show_error_messagebox();

  // This friendship allows using sigc++ signals although PG_Widget is derived
  // in a protected way.
//  friend class SigC::ObjectSlotNode;

  std::string m_error_message;
  ErrorType m_error_type;
  pthread_mutex_t m_error_lock;
  
  Window *m_child_window;
  pthread_mutex_t m_childwindow_lock;
  // TODO: m_childwindow_lock!!!!

  // This flag indicates that function do_running should end ASAP.
  bool m_end_run;
  int m_return_value;
  
};

class MessageBox  :  public PG_MessageBox
{
public:
  MessageBox(const std::string &message);
  inline bool is_closed() const { return this->m_closed; }
protected:
  virtual bool handleButton(PG_Button *button);
private:
  bool m_closed;
};

#endif /*WINDOW_HH_*/
