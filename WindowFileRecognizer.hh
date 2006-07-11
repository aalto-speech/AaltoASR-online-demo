
#ifndef WINDOWFILERECOGNIZER_HH_
#define WINDOWFILERECOGNIZER_HH_

#include <pgbutton.h>
#include "AudioFileInputController.hh"
#include "WindowRecognizer.hh"

class WindowFileRecognizer  :  public WindowRecognizer
{

public:
  
  WindowFileRecognizer(msg::InQueue *in_queue, OutQueueController *out_queue);
  virtual ~WindowFileRecognizer();
  
  virtual void initialize();
  
protected:

  virtual void do_opening();
  virtual void do_running();
  virtual void do_closing(int return_value);
  
  virtual AudioInputController* get_audio_input() { return this->m_audio_input; }
  
  virtual void reset(bool reset_audio);
  
  void open_audio_file();
  
  bool handle_button(PG_Button *button);
  
private:

  AudioFileInputController *m_audio_input;
  PG_Button *m_open_button;
  bool m_open_file;
};

#endif /*WINDOWFILERECOGNIZER_HH_*/
