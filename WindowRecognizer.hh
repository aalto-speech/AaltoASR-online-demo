
#ifndef WINDOWRECOGNIZER_HH_
#define WINDOWRECOGNIZER_HH_

#include "Window.hh"
#include "Process.hh"
#include "Recognition.hh"
#include "AudioFileInputController.hh"
#include "WidgetRecognitionArea.hh"
#include "QueueController.hh"
#include "msg.hh"
#include <pgbutton.h>

class WindowRecognizer  :  public Window
{

public:
  
  WindowRecognizer(Process *process,
                   msg::InQueue *in_queue,
                   msg::OutQueue *out_queue);
  virtual ~WindowRecognizer() { }
  
  virtual void initialize();
  
protected:

  enum Status { END_OF_AUDIO, LISTENING };

  virtual void do_opening();
  virtual void do_running();
  virtual void do_closing(int return_value);
  
  virtual AudioInputController* get_audio_input() = 0;
  
  PG_Button* construct_button(const std::string &label,
                              unsigned int column_index,
                              unsigned int row_index,
                              const SigC::Slot0<bool> &callback);
  
  void enable_recognizer(bool enable);
  
  bool handle_back_button();
  bool handle_play_button();
  bool handle_reset_button();
  bool handle_resetrecog_button();
  bool handle_enablerecog_button();
  bool handle_endaudio_button();
  bool handle_settings_button();
  bool handle_showtext_button();
  
  void set_status(Status status);
  inline Status get_status() const { return this->m_status; }
  
  void pause_audio_input(bool pause);
  virtual void reset(bool reset_audio);
  void end_of_audio();
  
  void flush_out_queue();
  
  void pause_window_functionality(bool pause);
  
  void handle_broken_pipe();

  Process *m_process;
  Recognition m_recognition;
  msg::InQueue *m_in_queue;
  msg::OutQueue *m_out_queue;
  QueueController m_queue;

  
private:

  PG_Button *m_play_button;
  PG_Button *m_endaudio_button;
  PG_Button *m_enablerecog_button;

  Status m_status;
  
  WidgetRecognitionArea *m_recognition_area;
};

#endif /*WINDOWRECOGNIZER_HH_*/
