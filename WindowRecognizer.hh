
#ifndef WINDOWRECOGNIZER_HH_
#define WINDOWRECOGNIZER_HH_

#include "Recognition.hh"
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

  enum Status { END_OF_AUDIO, LISTENING };

  virtual void do_opening();
  virtual void do_running();
  virtual void do_closing();
  
  virtual AudioInputController* get_audio_input() = 0;
  
  void parse_recognition(const std::string &message);

  bool button_pressed(PG_Button *button);
  
  void set_status(Status status);
  inline Status get_status() const { return this->m_status; }
  
  void pause_audio_input(bool pause);
  void reset_audio_input();
  void end_of_audio();
  
  Recognition m_recognition;
  
//  AudioFileInputController *m_audio_input;
  msg::InQueue *m_in_queue;
  OutQueueController *m_out_queue;

  
private:

  // T‰m‰n setin voisi heitt‰‰ omaan luokkaansa - alkaen t‰st‰ -->
  bool start_inqueue_thread();
  void stop_inqueue_thread();
  static void* in_queue_listener(void *user_data);
  pthread_t m_thread;
  bool m_end_thread;
  bool m_thread_created;
  // --> ja p‰‰ttyen t‰nne
  
  Status m_status;
  bool m_reset;
  bool m_paused;
  
  PG_Button *m_back_button;
  PG_Button *m_play_button;
  PG_Button *m_reset_button;
//  PG_Button *m_open_button;
//  WidgetSpectrum *m_spectrum;
  WidgetRecognitionArea *m_recognition_area;
};

#endif /*WINDOWRECOGNIZER_HH_*/
