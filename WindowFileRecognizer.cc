
#include "WindowFileRecognizer.hh"
#include "WindowReset.hh"
#include "WindowOpenAudioFile.hh"

WindowFileRecognizer::WindowFileRecognizer(RecognizerProcess *recognizer)//Process *process,
                                           //msg::InQueue *in_queue,
                                           //msg::OutQueue *out_queue)
  : WindowRecognizer(recognizer)
//  : WindowRecognizer(process, in_queue, out_queue)
{
  this->m_audio_input = NULL;
}

WindowFileRecognizer::~WindowFileRecognizer()
{
  delete this->m_audio_input;
}

void
WindowFileRecognizer::initialize()
{
  WindowRecognizer::initialize();
  
  this->construct_button("Open audio", 0, 2, slot(*this, &WindowFileRecognizer::handle_open_button));
}

void
WindowFileRecognizer::do_opening()
{
  this->m_audio_input = new AudioFileInputController(this->m_recognizer ? this->m_recognizer->get_out_queue() : NULL);
  if (!this->m_audio_input->initialize()) {
    this->error("Audio input controller initialization failed. Try closing all other programs.", ERROR_CLOSE);
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
WindowFileRecognizer::handle_open_button()
{
  WindowOpenAudioFile window(this->m_window, this->m_audio_input);
  int ret_val;
  
  this->pause_window_functionality(true);
  window.initialize();
  ret_val = this->run_child_window(&window);

  if (ret_val == 1) {
    this->pause_audio_input(true);
    this->m_recognition_area->set_scroll_position(0);
    // Run reset window without reseting audio.
    this->reset(false);

    if (!this->m_audio_input->is_eof()) {
      this->set_status(LISTENING);
    }
  }
  this->pause_window_functionality(false);
  return true;
}
