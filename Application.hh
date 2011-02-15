
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
   * \param argv0 The argv[0] string passed to your program's mainline. This
   * may be NULL on most platforms (such as ones without a standard main()
   * function), but you should always try to pass something in here. Unix-
   * like systems such as Linux _need_ to pass argv[0] from main() in here.
   * \param width Application window width.
   * \param height Application window height.
   * \return false if ParaGUI initialization failed. */
  bool initialize(const char * argv0, unsigned int width, unsigned int height);
  
  /** Initializes the application with recognizer.
   * \param argv0 The argv[0] string passed to your program's mainline. This
   * may be NULL on most platforms (such as ones without a standard main()
   * function), but you should always try to pass something in here. Unix-
   * like systems such as Linux _need_ to pass argv[0] from main() in here.
   * \param width Application window width.
   * \param height Application window height.
   * \param connect Optional SSH connection command,
   *                e.g. "ssh pyramid.hut.fi ssh itl-cl3".
   * \param script_file File that should be run in the ssh computer or locally.
   * \param beam Beam value.
   * \param lmscale LM-scale value.
   * \return false if ParaGUI initialization failed. */
  bool initialize(const char * argv0,
                  unsigned int width,
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
  
private:

  RecognizerProcess *m_recognizer; //!< Recognizer process.
  PG_Application *m_app; //!< ParaGUI application.
  
};

#endif /*APPLICATION_HH_*/
