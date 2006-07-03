
#ifndef WINDOWFILERECOGNIZER_HH_
#define WINDOWFILERECOGNIZER_HH_

#include "AudioFileInputController.hh"
#include "WindowRecognizer.hh"
#include "WidgetSpectrum.hh"
#include <pgbutton.h>

class WindowFileRecognizer  :  public WindowRecognizer
{

public:
  
  WindowFileRecognizer(msg::InQueue *in_queue, OutQueueController *out_queue);
  virtual ~WindowFileRecognizer();
  
  virtual void initialize();
  
protected:

  virtual void do_opening();
  virtual void do_running();
  virtual void do_closing();
  
  virtual AudioInputController* get_audio_input() { return this->m_audio_input; }
  
//  void do_reseting();

  bool button_pressed(PG_Button *button);
  
//  void pause_audio_input(bool pause);
//  void reset_audio_input();
  
//  bool start_inqueue_thread();  

//  void stop_inqueue_thread();
  
//  static void* in_queue_listener(void *user_data);
  
private:

  AudioFileInputController *m_audio_input;
//  msg::InQueue *m_in_queue;
//  OutQueueController *m_out_queue;
//  pthread_t m_thread;
//  bool m_end_thread;
//  bool m_thread_created;
  
//  bool m_reset;
  
//  PG_Button *m_back_button;
//  PG_Button *m_play_button;
//  PG_Button *m_reset_button;
  PG_Button *m_open_button;
//  WidgetSpectrum *m_spectrum;
};

#endif /*WINDOWFILERECOGNIZER_HH_*/
