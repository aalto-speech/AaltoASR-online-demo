
#include "WindowFileRecognizer.hh"
#include "WindowReset.hh"
#include "WindowOpenFile.hh"

WindowFileRecognizer::WindowFileRecognizer(Process *process,
                                           msg::InQueue *in_queue,
                                           msg::OutQueue *out_queue)
  : WindowRecognizer(process, in_queue, out_queue)
{
  this->m_open_button = NULL;
  this->m_audio_input = NULL;
}

WindowFileRecognizer::~WindowFileRecognizer()
{
  delete this->m_open_button;
  delete this->m_audio_input;
}

void
WindowFileRecognizer::initialize()
{
  WindowRecognizer::initialize();
  
  this->m_open_button = new PG_Button(this->m_window, PG_Rect(10,200,150,50), "Open file");
  this->m_window->AddChild(this->m_open_button);
  this->m_open_button->sigClick.connect(slot(*this, &WindowFileRecognizer::handle_open_button));
}

void
WindowFileRecognizer::do_opening()
{
  this->m_audio_input = new AudioFileInputController(this->m_out_queue);
  if (!this->m_audio_input->initialize()) {
    this->error("Audio input controller initialization failed.", ERROR_CLOSE);
    return;
  }
  WindowRecognizer::do_opening();
  this->set_status(END_OF_AUDIO);
}

void
WindowFileRecognizer::do_running()
{
  WindowRecognizer::do_running();

  if (this->get_status() != END_OF_AUDIO) {
    if (this->m_audio_input->is_eof())
      this->end_of_audio();
  }
}

void
WindowFileRecognizer::do_closing(int return_value)
{
  WindowRecognizer::do_closing(return_value);
  if (this->m_audio_input) {
    this->m_audio_input->terminate();
    delete this->m_audio_input;
    this->m_audio_input = NULL;
  }
}

void
WindowFileRecognizer::reset(bool reset_audio)
{
  WindowRecognizer::reset(reset_audio);
  
  if (this->m_audio_input->is_eof()) {
    this->set_status(END_OF_AUDIO);
  }
  else {
    this->set_status(LISTENING);
  }
}

bool
WindowFileRecognizer::handle_open_button(PG_Button *button)
{
  WindowOpenFile window(this->m_window, this->m_audio_input);
  int ret_val;
  
  this->pause_window_functionality(true);
  window.initialize();
  ret_val = this->run_child_window(&window);

  if (ret_val == 1) {
    // Run reset window without reseting audio.
    this->reset(false);

    if (!this->m_audio_input->is_eof()) {
      this->set_status(LISTENING);
    }
  }
  return true;
}
