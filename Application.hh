
#ifndef APPLICATION_HH_
#define APPLICATION_HH_

#include <pgapplication.h>
#include "RecognizerProcess.hh"
#include "Window.hh"

/**
 * Class for running the application. Works as a top-level window manager too.
 */
class Application
{
  
public:

  /** Contructs the application object. */
  Application();
  /** Destruction and cleaning. */
  virtual ~Application();

  /** Initializes the application without recognizer. This is mostly for
   * debugging purposes only.
   * \return false if ParaGUI initialization failed. */
  bool initialize();
  
  /** Initializes the application with recognizer.
   * \param ssh_to Computer which should run the recognizer.
   * \param script_file File that should be run in the ssh computer.
   * \return false if ParaGUI initialization failed. */
  bool initialize(const std::string &ssh_to, const std::string &script_file);
  
  /** Cleans memory allocations. */
  void clean();
  
  /** Runs the application. This function returns when the application has
   * finished. */
  void run();
  
protected:

  /** Selects next window to run according to current window and its return
   * value
   * \param ret_val Return value of run modal function of the current window.
   */
  void next_window(int ret_val);
  
private:

  // Recognizer process
  RecognizerProcess *m_recognizer;
  
  // Windowing system
  Window *m_current_window;
  Window *m_startprocess_window;
  Window *m_main_window;
  Window *m_recognizer_window;
  Window *m_microphone_window;

  // ParaGUI
  PG_Application *m_app;
  
};

#endif /*APPLICATION_HH_*/
