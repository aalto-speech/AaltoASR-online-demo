
#include "Application.hh"
#include "WindowStartProcess.hh"
#include "WindowRecognizer.hh"
#include "WindowSettings.hh"
//#include "scrap.h"



//Application* Application::app = NULL;

Application::Application()
{
  this->m_recognizer = NULL;
//  this->m_out_queue = NULL;
//  this->m_in_queue = NULL;
  
//  this->m_current_window = NULL;
//  this->m_startprocess_window = NULL;
//  this->m_main_window = NULL;
//  this->m_recognizer_window = NULL;
//  this->m_microphone_window = NULL;
  
  this->m_app = NULL;
  
//  Application::app = this;
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
    fprintf(stderr, "Width must be at least 950.\n", width, height);
    return false;
  }
  
  if (height < 600) {
    fprintf(stderr, "Width must be at least 600.\n", width, height);
    return false;
  }

  // Initialize PG_Application.
//  if(!this->m_app->InitScreen(1024, 768, 0, SDL_FULLSCREEN | SDL_SWSURFACE | SDL_DOUBLEBUF)) {
  if(!this->m_app->InitScreen(width, height)) {
    fprintf(stderr, "Resolution %dx%d not supported\n", width, height);
    return false;
  }
//  this->m_app->EnableAppIdleCalls(); 
//  this->m_app->SetEmergencyQuit(true);
  this->m_app->LoadTheme("default");
  this->m_app->SetCaption("Online-demo", NULL);
//  this->m_app->DeleteBackground();
  this->m_app->RedrawBackground(PG_Rect(0, 0, this->m_app->GetScreenWidth(), this->m_app->GetScreenHeight()));
  this->m_app->FlipPage();

  /*
  // Initialize windows.
  this->m_startprocess_window = new WindowStartProcess(NULL, this->m_recognizer);
//  this->m_main_window = new WindowMain();
  this->m_recognizer_window = new WindowRecognizer(this->m_recognizer);
//  this->m_microphone_window = new WindowRecognizer(this->m_recognizer);
//  this->m_recognizer_window = new WindowFileRecognizer(this->m_recognizer);
//  this->m_microphone_window = new WindowMicrophoneRecognizer(this->m_recognizer);
  
  this->m_startprocess_window->initialize();
  this->m_main_window->initialize();
  this->m_recognizer_window->initialize();
  this->m_microphone_window->initialize();
  //*/
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
  // Program starts in initialization window.
//  this->m_current_window = this->m_startprocess_window;
/*  
  char *scrap = NULL;
  int scraplen = 0;
  get_scrap(T('T','E','X','T'), &scraplen, &scrap);
  
  fprintf(stderr, "Clipboard content: %s\n", scrap);
//  for (int ind = 0; ind < scraplen; ind++) {
//    fprintf(stderr, "%c", scrap[ind]);
//  }
  delete[] scrap;
//  fprintf(stderr, "\n");
  return;
//*/
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
  /*
  // Clean windows.
  delete this->m_startprocess_window;
//  delete this->m_main_window;
  delete this->m_recognizer_window;
//  delete this->m_microphone_window;
  this->m_startprocess_window = NULL;
//  this->m_main_window = NULL;
  this->m_recognizer_window = NULL;
//  this->m_microphone_window = NULL;
//*/


  // Finish and clean recognizer process.
  if (this->m_recognizer) {
    this->m_recognizer->finish();
    delete this->m_recognizer;
    this->m_recognizer = NULL;
  }
  
  delete this->m_app;
  this->m_app = NULL;

}

/*
void
Application::next_window(int ret_val)
{
  if (this->m_current_window) {
    if (ret_val == 0) {
      this->m_current_window = NULL;
    }
    else if (this->m_current_window == this->m_startprocess_window) {
      this->m_current_window = this->m_main_window;
    }
    else if (this->m_current_window == this->m_main_window) {
      if (ret_val == 1) {
        this->m_current_window = this->m_recognizer_window;
      }
      else {
        this->m_current_window = this->m_microphone_window;
      }
    }
    else if (this->m_current_window == this->m_recognizer_window) {
      this->m_current_window = this->m_main_window;
    }
    else if (this->m_current_window == this->m_microphone_window) {
      this->m_current_window = this->m_main_window;
    }
    else {
      this->m_current_window = NULL;
    }
  }
}
//*/
