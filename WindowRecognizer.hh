
#ifndef WINDOWRECOGNIZER_HH_
#define WINDOWRECOGNIZER_HH_

#include "Window.hh"
//#include "Process.hh"
#include "RecognitionParser.hh"
#include "AudioInputController.hh"
#include "WidgetRecognitionArea.hh"
#include "WidgetComparisonArea.hh"
#include "RecognizerListener.hh"
//#include "msg.hh"
#include "RecognizerProcess.hh"
#include <pgbutton.h>
#include <pglabel.h>
#include <pgcheckbutton.h>
//#include "WidgetCheckButton.hh"

class WindowRecognizer  :  public Window
{

public:
  
//  WindowRecognizer(Process *process,
//                   msg::InQueue *in_queue,
//                   msg::OutQueue *out_queue);
  WindowRecognizer(RecognizerProcess *recognizer);
  virtual ~WindowRecognizer();
  
  virtual void initialize();
  
protected:

//  enum Status { END_OF_AUDIO, LISTENING };

  virtual void do_opening();
  virtual void do_running();
  virtual void do_closing(int return_value);
  
//  virtual AudioInputController* get_audio_input() = 0;
  
  PG_Rect calculate_button_rect(unsigned int column_index,
                                unsigned int row_index);
                              
  PG_Button* construct_button(const std::string &label,
                              unsigned int column_index,
                              unsigned int row_index,
                              const SigC::Slot0<bool> &callback);
  
  void enable_recognizer(bool enable);
  
  bool handle_stop_button();
  bool handle_record_button();
  bool handle_recognize_button();
  bool handle_pause_button();
  bool handle_back_button();
//  bool handle_play_button();
//  bool handle_reset_button();
  bool handle_resetrecog_button();
  bool handle_pauserecog_button();
//  bool handle_endaudio_button();
  bool handle_settings_button();
//  bool handle_adapt_check();
  bool handle_resetadaptation_button();
  bool handle_save_button();
  bool handle_open_button();
  bool handle_showadvanced_button();
  bool handle_key_event(const SDL_KeyboardEvent *key);
  
  void open_audio_file();

//  void set_status(Status status);
//  inline Status get_status() const { return this->m_status; }
  
  virtual void pause_audio_input(bool pause);
  virtual void reset(bool reset_audio);
  void end_of_audio();
  
  void flush_out_queue();
  
  void pause_window_functionality(bool pause);
  
  bool handle_broken_pipe();

//  Process *m_process;
//  msg::InQueue *m_in_queue;
//  msg::OutQueue *m_out_queue;
  RecognizerProcess *m_recognizer;
  RecognitionParser m_recognition;
  RecognizerListener m_queue;

  WidgetRecognitionArea *m_recognition_area;
  WidgetComparisonArea *m_texts_area;
  
private:

//  PG_Button *m_play_button;
//  PG_Button *m_endaudio_button;
  PG_Button *m_pauserecog_button;
//  PG_Button *m_adapt_button;
  PG_Button *m_resetadapt_button;
  PG_Button *m_showadvanced_button;
  PG_Button *m_record_button;
  PG_Button *m_recognize_button;
  PG_Button *m_stop1_button;
  PG_Button *m_stop2_button;
  PG_Button *m_pause_button;
  PG_Button *m_reset_button;
  PG_CheckButton *m_adapt_check;
  
  PG_Label *m_status_label;
  
  bool m_broken_pipe;
  
  AudioInputController *m_audio_input;

//  Status m_status;
};

#endif /*WINDOWRECOGNIZER_HH_*/
