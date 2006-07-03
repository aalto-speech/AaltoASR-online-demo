
#ifndef APPLICATION_HH_
#define APPLICATION_HH_

#include <paragui.h>
#include <pgapplication.h>
#include <pthread.h>
#include "Process.hh"
#include "OutQueueController.hh"
#include "WindowInit.hh"
#include "WindowMain.hh"
#include "WindowFileRecognizer.hh"
#include "WindowMicrophoneRecognizer.hh"

class Application  :  protected PG_Application
{
  
public:

  Application();
  virtual ~Application();

  bool initialize();
  void clean();
  void run();
  
protected:

  bool start_recognizer();
  static void* start_gui(void *user_data);
  virtual bool eventQuit(int id, PG_MessageObject *widget, unsigned long data);
  
//  Window* select_next_window(Window );
  
  Process *m_recognizer;
  OutQueueController *m_out_queue;
  msg::InQueue *m_in_queue;
  
  Window *m_current_window;
  WindowInit *m_init_window;
  WindowMain *m_main_window;
  WindowFileRecognizer *m_recognizer_window;
  WindowMicrophoneRecognizer *m_microphone_window;
  pthread_mutex_t m_lock;
  bool m_thread_finished;
//  bool m_end_run;
  
};

#endif /*APPLICATION_HH_*/
