
#ifndef WINDOW_HH_
#define WINDOW_HH_

#include <pgwindow.h>

/**
 * Window is an independent abstraction of widgets and functionality. It is
 * run in modal mode, so only one Window can be active at a time.
 */
class Window  :  public SigC::Object
{

public:

  Window();
  virtual ~Window();

  /** Run this window modally.
   * \return Window-specific closing value. Value 0 means that the whole program
   *         should exit. */
  int run_modal();
  
  /** Does some initialization that can't be done in constructor. Must be
   * called once after constructor. */
  virtual void initialize();
  
  /** Runs child window modally.
   * \param child_window Window to run on top of this window. */
  int run_child_window(Window *child_window);
  
  /** Returns the window widget.
   * \return Window widget. */
  inline PG_Widget* get_widget() { return this->m_window; }

  /** Tells the action to take on error.
   * ERROR_FATAL exits the application
   * ERROR_CLOSE closes this window
   * ERROR_NORMAL does nothing */
  enum ErrorType { ERROR_FATAL, ERROR_CLOSE, ERROR_NORMAL };

  /** Pops a message box with error report.
   * \param message Error report.
   * \param type Tells the action to take.*/
  bool error(const std::string &message, ErrorType type);
  
protected:

  /** Does opening procedures. Must be called once before do_running calls. */
  virtual void do_opening() { };
  /** Does some fast procedures. This function is called repeatedly. */
  virtual void do_running() { };
  /** Does closing procedures. Must be called once after do_running calls.
   * \param return_value The value to return from run_modal function. */
  virtual void do_closing(int return_value) { };
  
  /** Requests the window to close (stop calling do_running).
   * \param return_value Value to return from the run_modal function. Zero means
   *                     that the whole program should be exited. */
  void end_running(int return_value);
  
  /** Disables or enables the window, so it won't disturb e.g. child window.
   * \param pause true to disable, false to enable.*/
  virtual void pause_window_functionality(bool pause);

  /** Creates the widget for the window. Allocates the memory, but freeing
   * is done in destructor. This function is called from initialize function. */  
  virtual PG_Widget* create_window();
  
  PG_Widget *m_window;
  
private:

  /** Does some default opening procedures and calls do_opening to allow
   * opening procedures for derived classes. */
  void open();
  /** Does some default closing procedures and calls do_closing to allow
   * closing procedures for derived classes. */
  void close();

  /** Flag to indicate that window should stop running modal and close. */
  bool m_end_run;
  /** Closed window returns this value to allow other windows to interact.
   * Zero value means that application should exit. */
  int m_return_value;
  
};

#endif /*WINDOW_HH_*/
