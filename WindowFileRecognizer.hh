
#ifndef WINDOWFILERECOGNIZER_HH_
#define WINDOWFILERECOGNIZER_HH_

#include "undefine.hh"
#include <pgbutton.h>
#include "AudioFileInputController.hh"
#include "WindowRecognizer.hh"

class WindowFileRecognizer  :  public WindowRecognizer
{

public:
  
  WindowFileRecognizer(Process *process, msg::InQueue *in_queue, msg::OutQueue *out_queue);
  virtual ~WindowFileRecognizer();
  
  virtual void initialize();
  
protected:

  virtual void do_opening();
  virtual void do_running();
  virtual void do_closing(int return_value);
  
  virtual AudioInputController* get_audio_input() { return this->m_audio_input; }
  
  virtual void reset(bool reset_audio);
  
  void open_audio_file();
  
  bool handle_open_button();
  
private:

  AudioFileInputController *m_audio_input;

};

#endif /*WINDOWFILERECOGNIZER_HH_*/
