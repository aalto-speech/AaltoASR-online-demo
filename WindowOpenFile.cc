
#include "WindowOpenFile.hh"

WindowOpenFile::WindowOpenFile(int x, int y, AudioFileInputController *audio_input)
{
  this->m_x = x;
  this->m_y = y;
  this->m_audio_input = audio_input;
  this->m_filename_textbox = NULL;
  this->m_ok_button = NULL;
  this->m_cancel_button = NULL;
  this->m_load = false;
}

WindowOpenFile::~WindowOpenFile()
{
  delete this->m_filename_textbox;
  delete this->m_ok_button;
  delete this->m_cancel_button;
}

void
WindowOpenFile::initialize()
{
  Window::initialize();
  
  this->m_filename_textbox = new PG_LineEdit(this->m_window, PG_Rect(10,30,100,20));
  this->m_ok_button = new PG_Button(this->m_window, PG_Rect(10,100,60,20), "OK");
  this->m_cancel_button = new PG_Button(this->m_window, PG_Rect(100,100,60,20), "Cancel");
  
  this->m_window->AddChild(this->m_filename_textbox);
  this->m_window->AddChild(this->m_ok_button);
  this->m_window->AddChild(this->m_cancel_button);
  
  this->m_ok_button->sigClick.connect(slot(*this, &WindowOpenFile::handle_button));
  this->m_cancel_button->sigClick.connect(slot(*this, &WindowOpenFile::handle_button));
  
  // This is to fasten debugging..
  this->m_filename_textbox->SetText("chunk.wav");
}

void
WindowOpenFile::do_opening()
{
  this->m_filename_textbox->EditBegin();
  this->m_load = false;
}

void
WindowOpenFile::do_running()
{
  if (this->m_load) {
    this->m_filename_textbox->EditEnd();
    // Do reseting and file loading.
    if (this->m_audio_input->load_file(this->m_filename_textbox->GetText())) {
      this->m_filename_textbox->EditEnd();
      this->close(1);
    }
    else {
      this->error("WOF audio file loading failed.\n", ERROR_NORMAL);
      this->m_load = false;
    }
  }
}

void
WindowOpenFile::do_closing(int return_value)
{
  /*
  if (return_value == 1) {
    // Do reseting and file loading.
//    this->m_audio_input->reset
    if (!this->m_audio_input->load_file(this->m_filename_textbox->GetText())) {
      fprintf(stderr, "WOF audio file loading failed.\n");
    }
  }
  //*/
}

bool
WindowOpenFile::handle_button(PG_Button *button)
{
  if (button == this->m_ok_button) {
    this->m_load = true;
  }
  else if (button == this->m_cancel_button) {
    this->close(2);
  }
  return true;
}

