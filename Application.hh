
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
   * \param width Application window width.
   * \param height Application window height.
   * \return false if ParaGUI initialization failed. */
  bool initialize(unsigned int width, unsigned int height);
  
  /** Initializes the application with recognizer.
   * \param width Application window width.
   * \param height Application window height.
   * \param connect Optional SSH connection command,
   *                e.g. "ssh pyramid.hut.fi ssh itl-cl3".
   * \param script_file File that should be run in the ssh computer.
   * \param beam Beam value.
   * \param lmscale LM-scale value.
   * \return false if ParaGUI initialization failed. */
  bool initialize(unsigned int width,
                  unsigned int height,
                  const std::string &connect,
                  const std::string &script_file,
                  unsigned int beam,
                  unsigned int lmscale) throw(Exception);
  
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
//  void next_window(int ret_val);
  
private:

  // Recognizer process
  RecognizerProcess *m_recognizer;
  
  // Windowing system
//  Window *m_current_window;
//  Window *m_startprocess_window;
//  Window *m_main_window;
//  Window *m_recognizer_window;
//  Window *m_microphone_window;

  // ParaGUI
  PG_Application *m_app;
  
};

#endif /*APPLICATION_HH_*/
