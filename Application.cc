
#include "Application.hh"

Application::Application()
{
  this->m_recognizer = NULL;
  this->m_out_queue = NULL;
  this->m_in_queue = NULL;
  
  this->m_current_window = NULL;
  this->m_init_window = NULL;
  this->m_main_window = NULL;
  this->m_recognizer_window = NULL;
  this->m_microphone_window = NULL;
  
  pthread_mutex_init(&this->m_lock, NULL);
}

Application::~Application()
{
  pthread_mutex_destroy(&this->m_lock);
  this->clean();
}

void
Application::run()
{
  int ret_val;
  Window *next_window;
  pthread_t thread;

  // This flag tells when this function is waiting for gui thread.
  this->m_thread_finished = false;
  
  // Program starts in initialization window.
  this->m_current_window = this->m_init_window;
  
  // This lock prevents gui thread from sending quit signal at unwanted time.
  pthread_mutex_lock(&this->m_lock);

  // Start GUI thread.
  pthread_create(&thread, NULL, Application::start_gui, this);
  
//  if (this->m_out_queue)
//    this->m_out_queue->start_flushing();
  
  // "Main loop"
  while (this->m_current_window) {
    // Open call should be made before the actual run call.
    this->m_current_window->open();
    // For thread-safety..
    next_window = this->m_current_window;
    
    // Between these unlock and lock is the place where the gui thread
    // may send quit signal to the window and set m_current_window to null.
    pthread_mutex_unlock(&this->m_lock);
    ret_val = next_window->run();
    pthread_mutex_lock(&this->m_lock);

    // Choose the next window.    
    if (ret_val == 0) {
      this->m_current_window = NULL;
    }
    else if (this->m_current_window == this->m_init_window) {
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
  pthread_mutex_unlock(&this->m_lock);

//  if (this->m_out_queue)
//    this->m_out_queue->stop_flushing();

  fprintf(stderr, "Wait for GUI to finish..\n");
  this->m_thread_finished = true;
  this->Quit();
  pthread_join(thread, NULL);
  // At this point you can be sure that also the gui thread has finished.
}

void*
Application::start_gui(void *user_data)
{
  Application *app = (Application*)user_data;

  // This fixes the dummy quitting behaviour!
  app->EnableAppIdleCalls();
  // This call returns after GUI has finished.
  app->Run();

  fprintf(stderr, "GUI main loop finished.\n");
  return NULL;  
}

bool
Application::start_recognizer()
{
  if (this->m_recognizer->create() == 0) {
    int ret = execlp(//"./rec.sh", "script",
    "ssh", "ssh", "itl-cl3", "/home/jluttine/workspace/online-demo/rec.sh",
//                    "./recognizer",
    //./recognizer", "./recognizer",
//                    "--config", "mfcc_p_dd.feaconf",
                    (char*)NULL);
    if (ret < 0) {
      perror("Application::start_recognizer exec() failed");
      return false;                                    //*/
      
    }
    assert(false);
  }
  
  msg::set_non_blocking(this->m_recognizer->read_fd);
  msg::set_non_blocking(this->m_recognizer->write_fd);
//  in_queue->fd = proc->read_fd;
//  out_queue->fd = proc->write_fd;
  return true;
  
}

bool
Application::eventQuit(int id, PG_MessageObject *widget, unsigned long data)
{
  // Send quitting signal to windowing thread.
  pthread_mutex_lock(&this->m_lock);
  if (this->m_current_window) {
    this->m_current_window->quit();
    this->m_current_window = NULL;
  }
  pthread_mutex_unlock(&this->m_lock);
  
  // Wait for windowing thread to finish. This is just to make quitting totally
  // safe.
  while (!this->m_thread_finished) {
    pthread_yield();
  }
  
  return PG_Application::eventQuit(id, widget, data);
}

bool
Application::initialize()
{
  // Initialize PG_Application.
  if(!this->InitScreen(1024, 900)){//, 0, SDL_FULLSCREEN)){// | SDL_SWSURFACE | SDL_DOUBLEBUF)){
    fprintf(stderr, "Resolution not supported\n");
    return false;
  }
  this->LoadTheme("default");
  this->SetCaption("Online-demo", NULL);

  // Initialize and start recognizer process.
/* Commenting these lines out will disable recognizer.
  this->m_recognizer = new Process();
  if (!this->start_recognizer()) {
    fprintf(stderr, "Application::initialize couldn't start recognizer process.\n");
    return false;
  }
  this->m_out_queue = new OutQueueController(this->m_recognizer->write_fd);
  this->m_in_queue = new msg::InQueue(this->m_recognizer->read_fd);
//*/

  // Initialize windows.
  this->m_init_window = new WindowInit(this->m_in_queue);
  this->m_main_window = new WindowMain();
  this->m_recognizer_window = new WindowFileRecognizer(this->m_in_queue, this->m_out_queue);
  this->m_microphone_window = new WindowMicrophoneRecognizer(this->m_in_queue, this->m_out_queue);
  
  this->m_init_window->initialize();
  this->m_main_window->initialize();
  this->m_recognizer_window->initialize();
  this->m_microphone_window->initialize();
  
  return true;
}

void
Application::clean()
{
  // Do cleaning in reverse order compared to initialization.
  
  // Clean windows.
  delete this->m_init_window;
  delete this->m_main_window;
  delete this->m_recognizer_window;
  delete this->m_microphone_window;
  this->m_init_window = NULL;
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
}

