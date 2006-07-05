
#include "WindowFileRecognizer.hh"
#include "WindowReset.hh"

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
  this->m_open_button->sigClick.connect(slot(*this, &WindowFileRecognizer::button_pressed));
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
}

void
WindowFileRecognizer::do_running()
{
  WindowRecognizer::do_running();

  if (this->get_status() != END_OF_AUDIO && this->m_audio_input->is_eof()) {
    this->end_of_audio();
  }
    
  if (this->m_open_file) {
    this->open_audio_file();
    this->m_open_file = false;
  }
  
}

void
WindowFileRecognizer::do_closing()
{
  WindowRecognizer::do_closing();
  if (this->m_audio_input) {
    this->m_audio_input->terminate();
    delete this->m_audio_input;
    this->m_audio_input = NULL;
  }
}
/*
void
WindowFileRecognizer::pause_audio_input(bool pause)
{
  if (this->m_audio_input)
    WindowRecognizer::pause_audio_input(this->m_audio_input->is_eof() || pause);
  else
    WindowRecognizer::pause_audio_input(pause);
}
//*/
void
WindowFileRecognizer::open_audio_file()
{
  this->pause_audio_input(true);
  this->reset_audio_input();
  this->m_audio_input->load_file("chunk.wav");
  if (!this->m_audio_input->is_eof()) {
    this->set_status(LISTENING);
  }
/*  else {
    // Just in case
    this->set_status(END_OF_AUDIO);
  }//*/
}

//*/
bool
WindowFileRecognizer::button_pressed(PG_Button *button)
{
  if (button == this->m_open_button) {
    this->m_open_file = true;
  }

  return true;
}
