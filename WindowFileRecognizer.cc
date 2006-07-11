
#include "WindowFileRecognizer.hh"
#include "WindowReset.hh"
#include "WindowOpenFile.hh"

WindowFileRecognizer::WindowFileRecognizer(msg::InQueue *in_queue, OutQueueController *out_queue)
  : WindowRecognizer(in_queue, out_queue)
//    m_spectrum(this, PG_Rect(100, 300, 500, 200))
{
/*
  this->m_back_button = NULL;
  this->m_play_button = NULL;
  this->m_reset_button = NULL;
  //*/
  this->m_open_button = NULL;
  this->m_open_file = false;
  
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
  this->m_open_button->sigClick.connect(slot(*this, &WindowFileRecognizer::handle_button));
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
  /*
  else {
    if (!this->m_audio_input->is_eof())
      this->set_status(LISTENING);
  }
  //*/
    
  if (this->m_open_file) {
    this->pause_window_functionality(true);
    this->open_audio_file();
    this->m_open_file = false;
    this->pause_window_functionality(false);
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

void
WindowFileRecognizer::open_audio_file()
{
  WindowOpenFile window(this->m_window, this->m_audio_input);

  window.initialize();
  fprintf(stderr, "before run child\n");
  if (this->run_child_window(&window) == 1) {
    fprintf(stderr, "before reset\n");
    // Run reset window without reseting audio.
    this->reset(false);

    fprintf(stderr, "after reset\n");

    if (!this->m_audio_input->is_eof()) {
      this->set_status(LISTENING);
      fprintf(stderr, "WFR audio file opening successful.\n");
    }
  }
}

//*/
bool
WindowFileRecognizer::handle_button(PG_Button *button)
{
  if (button == this->m_open_button) {
    this->m_open_file = true;
  }

  return true;
}
