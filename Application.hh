
#ifndef APPLICATION_HH_
#define APPLICATION_HH_

#include "undefine.hh"
#include <paragui.h>
#include "undefine.hh"
#include <pgapplication.h>
#include <pthread.h>
#include "Process.hh"
#include "msg.hh"
//#include "WindowInit.hh"
#include "WindowStartProcess.hh"
#include "WindowMain.hh"
#include "WindowFileRecognizer.hh"
#include "WindowMicrophoneRecognizer.hh"

class Application
{
  
public:

  Application();
  virtual ~Application();

  bool initialize(const std::string &ssh_to, const std::string &script_file);
  void clean();
  void run();
  
//  virtual void eventIdle();
  
protected:

  void next_window(int ret_val);
//  void next_window2();

//  void start_recognizer();
//  static void* start_gui(void *user_data);
//  virtual bool eventQuit(int id, PG_MessageObject *widget, unsigned long data);
  
//  Window* select_next_window(Window );
  
  Process *m_recognizer;
  msg::OutQueue *m_out_queue;
  msg::InQueue *m_in_queue;
  
  Window *m_current_window;
//  WindowInit *m_init_window;
  WindowStartProcess *m_startprocess_window;
  WindowMain *m_main_window;
  WindowFileRecognizer *m_recognizer_window;
  WindowMicrophoneRecognizer *m_microphone_window;
//  pthread_mutex_t m_lock;
//  bool m_thread_finished;

private:

  PG_Application *m_app;
  
};

#endif /*APPLICATION_HH_*/
