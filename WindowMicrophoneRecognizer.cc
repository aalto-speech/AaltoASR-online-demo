
#include "WindowMicrophoneRecognizer.hh"
#include "WindowSaveFile.hh"

WindowMicrophoneRecognizer::WindowMicrophoneRecognizer(Process *process,
                                                       msg::InQueue *in_queue,
                                                       msg::OutQueue *out_queue)
  : WindowRecognizer(process, in_queue, out_queue)
{
//  this->m_savefile_window = NULL;
  this->m_audio_input = NULL;
  this->m_save_button = NULL;
//  this->m_save_pressed = false;
}

WindowMicrophoneRecognizer::~WindowMicrophoneRecognizer()
{
  delete this->m_audio_input;
}

void
WindowMicrophoneRecognizer::initialize()
{
  WindowRecognizer::initialize();
  // ...  
  this->m_save_button = new PG_Button(this->m_window, PG_Rect(10,200,150,50), "Save");
  this->m_save_button->sigClick.connect(slot(*this, &WindowMicrophoneRecognizer::handle_save_button));
  this->m_window->AddChild(this->m_save_button);
}

void
WindowMicrophoneRecognizer::do_opening()
{
  this->m_audio_input = new AudioLineInputController(this->m_out_queue);
  if (!this->m_audio_input->initialize()) {
    this->error("Audio input controller initialization failed.", ERROR_CLOSE);
    return;
  }
  WindowRecognizer::do_opening();
}

void
WindowMicrophoneRecognizer::do_closing(int return_value)
{
  WindowRecognizer::do_closing(return_value);
  if (this->m_audio_input) {
    this->m_audio_input->terminate();
    delete this->m_audio_input;
    this->m_audio_input = NULL;
  }
}

void
WindowMicrophoneRecognizer::reset(bool reset_audio)
{
  WindowRecognizer::reset(reset_audio);
  this->set_status(LISTENING);
} 
/*
void
WindowMicrophoneRecognizer::save_audio_file()
{
}
//*/
/*
void
WindowMicrophoneRecognizer::handle_close_child_window(Window *child_window, int ret_val)
{
  Window::handle_close_child_window(child_window, ret_val);
  
  if (child_window == this->m_savefile_window) {
    this->pause_window_functionality(false);
    delete this->m_savefile_window;
    this->m_savefile_window = NULL;
  }
}
//*/
bool
WindowMicrophoneRecognizer::handle_save_button(PG_Button *button)
{
  this->pause_window_functionality(true);

  if (this->m_audio_input->get_audio_data_size()) {
    WindowSaveFile window(this->m_window, this->m_audio_input);
    window.initialize();
    this->run_child_window(&window);
  }
  else {
    this->error("No audio to save. Record some audio first.", ERROR_NORMAL);
  }

  this->pause_window_functionality(false);

  return true;
}
