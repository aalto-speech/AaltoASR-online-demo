
#include "Application.hh"
#include "WindowStartProcess.hh"
#include "WindowRecognizer.hh"
#include "WindowSettings.hh"

std::string find_theme_path() {
	enum { BUFFERSIZE = 1024 };
	char buf[BUFFERSIZE];
	ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf)-1);

	if (len != -1) {
	  buf[len] = '\0';
	}
	else {
	  exit(1);
	}
	std::string path =  std::string(buf);
	int beginIdx = path.rfind('/');
	return path.substr(0, beginIdx) + "/../share/aalto-online-demo/theme";
}

Application::Application()
{
  this->m_recognizer = NULL;
  this->m_app = NULL;
}

Application::~Application()
{
  this->clean();
}

bool
Application::initialize(unsigned int width, unsigned int height)
{
  this->m_app = new PG_Application;
  
  if (width < 950) {
    fprintf(stderr, "Width must be at least 950.\n");
    return false;
  }
  
  if (height < 600) {
    fprintf(stderr, "Width must be at least 600.\n");
    return false;
  }

  // Initialize PG_Application.
  if(!this->m_app->InitScreen(width, height)) {
    fprintf(stderr, "Resolution %dx%d not supported\n", width, height);
    return false;
  }
  fprintf(stderr, "theme path: %s\n", find_theme_path().c_str());
  this->m_app->LoadTheme("demo", true, find_theme_path().c_str());
  this->m_app->SetCaption("Online-demo", "");
  this->m_app->RedrawBackground(PG_Rect(0, 0, this->m_app->GetScreenWidth(), this->m_app->GetScreenHeight()));
  this->m_app->FlipPage();

  return true;
}

bool
Application::initialize(unsigned int width,
                        unsigned int height,
                        const std::string &connect,
                        const std::string &script_file,
                        unsigned int beam,
                        unsigned int lmscale) throw(Exception)
{
  // Create the recognizer.
  this->m_recognizer = new RecognizerProcess(connect, script_file, beam, lmscale);

  return this->initialize(width, height);
}

void
Application::run()
{
  try {
    int ret_val = 1;
    if (this->m_recognizer) {
      // Run recognizer starting window.
      Window *start_window = new WindowStartProcess(NULL, this->m_recognizer);
      start_window->initialize();
      ret_val = start_window->run_modal();
      delete start_window;
      /* These lines may be run if settings window is wanted to show in startup.
      if (ret_val != 0) {
        // Run settings window.
        Window *settings_window = new WindowSettings(NULL, this->m_recognizer);
        settings_window->initialize();
        ret_val = settings_window->run_modal();
        delete settings_window;
      }
      //*/
    }
    
    // If application is not finished, run actual recognizer window.
    if (ret_val != 0) {
      Window *recognizer_window = new WindowRecognizer(this->m_recognizer);
      recognizer_window->initialize();
      recognizer_window->run_modal();
      delete recognizer_window;
    }
  }
  catch (msg::ExceptionBrokenPipe exception) {
    fprintf(stderr, "Error: Unhandled broken pipe exception. Program exits.\n");
  }
  catch (Exception exception) {
    fprintf(stderr, "Error: Unhandled exception: %s\n", exception.what());
  }
  catch (std::exception exception) {
    fprintf(stderr, "Error: Unhandled exception: %s\n", exception.what());
  }
}

void
Application::clean()
{
  // Do cleaning in reverse order compared to initialization.

  // Finish and clean recognizer process.
  if (this->m_recognizer) {
    this->m_recognizer->finish();
    delete this->m_recognizer;
    this->m_recognizer = NULL;
  }
  
  delete this->m_app;
  this->m_app = NULL;

}
