
#ifndef WINDOWRECOGNIZER_HH_
#define WINDOWRECOGNIZER_HH_

#include "AudioFileInputController.hh"
#include "Window.hh"
#include "WidgetSpectrum.hh"
#include "WidgetRecognitionArea.hh"
#include <pgbutton.h>

class WindowRecognizer  :  public Window
{

public:
  
  WindowRecognizer(msg::InQueue *in_queue, OutQueueController *out_queue);
  virtual ~WindowRecognizer();
  
  virtual void initialize();
  
protected:

  virtual void do_opening();
  virtual void do_running();
  virtual void do_closing();
  
  virtual AudioInputController* get_audio_input() = 0;
  
  void do_reseting();

//  virtual bool eventKeyDown(const SDL_KeyboardEvent *key);
  
  bool button_pressed(PG_Button *button);
  
  void pause_audio_input(bool pause);
  void reset_audio_input();
  
//  AudioFileInputController *m_audio_input;
  msg::InQueue *m_in_queue;
  OutQueueController *m_out_queue;

private:

  bool start_inqueue_thread();
  void stop_inqueue_thread();
  static void* in_queue_listener(void *user_data);
  
  pthread_t m_thread;
  bool m_end_thread;
  bool m_thread_created;
  
  bool m_reset;
  
  PG_Button *m_back_button;
  PG_Button *m_play_button;
  PG_Button *m_reset_button;
//  PG_Button *m_open_button;
  WidgetSpectrum *m_spectrum;
  WidgetRecognitionArea *m_recognition_area;
};

#endif /*WINDOWRECOGNIZER_HH_*/
