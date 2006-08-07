
#include "Application.hh"
#include "Settings.hh"
#include "WindowStartProcess.hh"
#include "WindowMain.hh"
#include "WindowFileRecognizer.hh"
#include "WindowMicrophoneRecognizer.hh"

//Application* Application::app = NULL;

Application::Application()
{
  this->m_recognizer = NULL;
  this->m_out_queue = NULL;
  this->m_in_queue = NULL;
  
  this->m_current_window = NULL;
  this->m_startprocess_window = NULL;
  this->m_main_window = NULL;
  this->m_recognizer_window = NULL;
  this->m_microphone_window = NULL;
  
  this->m_app = NULL;
  
//  Application::app = this;
}

Application::~Application()
{
  this->clean();
}

bool
Application::initialize()
{
  this->m_app = new PG_Application;
  
  // Initialize PG_Application.
  if(!this->m_app->InitScreen(1024, 768)){//, 0, SDL_FULLSCREEN)){// | SDL_SWSURFACE | SDL_DOUBLEBUF)){
    fprintf(stderr, "Resolution not supported\n");
    return false;
  }
  this->m_app->EnableAppIdleCalls();
  this->m_app->LoadTheme("default");
  this->m_app->SetCaption("Online-demo", NULL);
  this->m_app->SetEmergencyQuit(true);
//  this->m_app->DeleteBackground();
  this->m_app->RedrawBackground(PG_Rect(0,0,this->m_app->GetScreenWidth(),this->m_app->GetScreenHeight()));
  this->m_app->FlipPage();
//  this->m_app->SetBulkMode(true);
  
  // Initialize windows.
  this->m_startprocess_window = new WindowStartProcess(NULL, this->m_recognizer, this->m_in_queue, this->m_out_queue);
  this->m_main_window = new WindowMain();
  this->m_recognizer_window = new WindowFileRecognizer(this->m_recognizer, this->m_in_queue, this->m_out_queue);
  this->m_microphone_window = new WindowMicrophoneRecognizer(this->m_recognizer, this->m_in_queue, this->m_out_queue);
  
  this->m_startprocess_window->initialize();
  this->m_main_window->initialize();
  this->m_recognizer_window->initialize();
  this->m_microphone_window->initialize();
  
  return true;
}

bool
Application::initialize(const std::string &ssh_to,
                        const std::string &script_file)
{
  // Set some settings.
  Settings::ssh_to = ssh_to;
  Settings::script_file = script_file;
  Settings::read_settings();

  // Initialize recognizer process.
//* Commenting these lines out will disable recognizer.
  this->m_recognizer = new Process();
  this->m_out_queue = new msg::OutQueue();
  this->m_in_queue = new msg::InQueue();
//*/

  return this->initialize();
}

void
Application::run()
{
  // Program starts in initialization window.
  this->m_current_window = this->m_startprocess_window;
  
  try {
    while (this->m_current_window) {
      int ret_val = this->m_current_window->run_modal();
      this->next_window(ret_val);
    }
  }
  catch (msg::ExceptionBrokenPipe exception) {
    fprintf(stderr, "Warning: Unhandled broken pipe exception. Program exits.\n");
  }
  /*
  catch (ExceptionChildProcess) {
    
    fprintf(stderr, "cleaning...\n");
    this->clean();
    
    int ret = execlp("ssh",
                     "ssh",
                     Settings::ssh_to.data(),
                     Settings::script_file.data(),
                     (char*)NULL);
                    
    if (ret < 0) {
      perror("Application::run exec() failed");
      exit(1);                                    
    }
    assert(false);
  }
  //*/
}

void
Application::clean()
{
  // Do cleaning in reverse order compared to initialization.
  
  // Clean windows.
  delete this->m_startprocess_window;
  delete this->m_main_window;
  delete this->m_recognizer_window;
  delete this->m_microphone_window;
  this->m_startprocess_window = NULL;
  this->m_main_window = NULL;
  this->m_recognizer_window = NULL;
  this->m_microphone_window = NULL;

  // Finish and clean recognizer process.
  delete this->m_out_queue;
  delete this->m_in_queue;
  this->m_out_queue = NULL;
  this->m_in_queue = NULL;
  if (this->m_recognizer) {
    if (this->m_recognizer->is_created()) {
      this->m_recognizer->finish();
    }
    delete this->m_recognizer;
    this->m_recognizer = NULL;
  }
  
  delete this->m_app;
  this->m_app = NULL;

}

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
