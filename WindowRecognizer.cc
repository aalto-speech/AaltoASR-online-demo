
#include <pgapplication.h>
#include "WindowRecognizer.hh"
#include "WindowMessageBox.hh"
#include "WindowReset.hh"
#include "WindowSettings.hh"
#include "WindowStartProcess.hh"
#include "WindowOpenAudioFile.hh"
#include "WindowSaveAudioFile.hh"
#include "str.hh"

WindowRecognizer::WindowRecognizer(RecognizerProcess *recognizer)
  : m_queue(recognizer ? recognizer->get_in_queue() : NULL, &m_recognition)
{
  this->m_audio_input = NULL;
  
  // Normal buttons
  this->m_record_button = NULL;
  this->m_recognize_button = NULL;
  this->m_pause_button = NULL;
  this->m_stop1_button = NULL;
  this->m_stop2_button = NULL;
  
  // Advanced buttons.
  this->m_showadvanced_button = NULL;
  this->m_adapt_check = NULL;
  this->m_resetadapt_button = NULL;
  this->m_pauserecog_button = NULL;
  this->m_reset_button = NULL;

  this->m_status_label = NULL;

  this->m_recognition_area = NULL;
  this->m_texts_area = NULL;
  
  this->m_recognizer = recognizer;
  
  this->m_broken_pipe = false;
  
}

WindowRecognizer::~WindowRecognizer()
{
  delete this->m_audio_input;
}

void
WindowRecognizer::initialize()
{
  Window::initialize();

  this->m_pauserecog_button =
    this->construct_button("Pause recognizer", 3, 2, slot(*this, &WindowRecognizer::handle_pauserecog_button));
  this->m_record_button =
    this->construct_button("RECORD", 0, 0, slot(*this, &WindowRecognizer::handle_record_button));
  this->m_recognize_button =
    this->construct_button("RECOGNIZE", 0, 1, slot(*this, &WindowRecognizer::handle_recognize_button));
  this->m_pause_button =
    this->construct_button("PAUSE", 0, 2, slot(*this, &WindowRecognizer::handle_pause_button));
  this->m_stop1_button =
    this->construct_button("STOP", 0, 0, slot(*this, &WindowRecognizer::handle_stop_button));
  this->m_stop2_button =
    this->construct_button("STOP", 0, 1, slot(*this, &WindowRecognizer::handle_stop_button));
  this->m_reset_button =
//  this->construct_button("Reset", 3, 3, slot(*this, &WindowRecognizer::handle_reset_button));
    this->construct_button("Reset recognizer", 3, 3, slot(*this, &WindowRecognizer::handle_resetrecog_button));
  this->construct_button("Settings..", 2, 0, slot(*this, &WindowRecognizer::handle_settings_button));
  this->construct_button("Exit", 1, 2, slot(*this, &WindowRecognizer::handle_back_button));
  this->m_adapt_check = new PG_CheckButton(this->m_window,
                                              this->calculate_button_rect(3, 0),
                                              "Adapt speaker");
    //this->construct_button("Adapt speaker", 3, 0, slot(*this, &WindowRecognizer::handle_adapt_button));
  this->m_resetadapt_button =
    this->construct_button("Reset adaptation", 3, 1, slot(*this, &WindowRecognizer::handle_resetadaptation_button));
  this->construct_button("Load audio", 1, 0, slot(*this, &WindowRecognizer::handle_open_button));
  this->construct_button("Save audio", 1, 1, slot(*this, &WindowRecognizer::handle_save_button));
  this->m_showadvanced_button =
    this->construct_button("Show advanced", 2, 2, slot(*this, &WindowRecognizer::handle_showadvanced_button));

  this->m_pauserecog_button->SetToggle(true);
//  this->m_adapt_button->SetToggle(true);
  this->m_pause_button->SetToggle(true);
  this->m_showadvanced_button->SetToggle(true);
  
//  this->m_adapt_check->sigClick.connect(slot(*this, &WindowRecognizer::handle_adapt_check));
  this->m_window->sigKeyUp.connect(slot(*this, &WindowRecognizer::handle_key_event));
  this->m_window->sigKeyDown.connect(slot(*this, &WindowRecognizer::handle_key_event));
  
  this->m_status_label = new PG_Label(this->m_window,
                                      PG_Rect(10,
                                              this->m_window->Height() - 25,
                                              this->m_window->Width() - 10,
                                              20),
                                      "Recognizer status: ");

}

void
WindowRecognizer::do_opening()
{
  this->m_audio_input = new AudioInputController(this->m_recognizer ? this->m_recognizer->get_out_queue() : NULL);
  if (!this->m_audio_input->initialize()) {
    this->error("Audio input controller initialization failed. Try closing all other programs.", ERROR_CLOSE);
    return;
  }

  // These constants are used to construct the following areas.
  const unsigned int top = 140;
  const unsigned int space = 5;
  const unsigned int bottom = 50;
  const unsigned int height = this->m_window->Height() - (top + space + bottom);
  const float text_part = 0.4;
  const float recognizer_part = 1.0 - text_part;

  // Create area for original and recognized texts.
  this->m_texts_area = new WidgetComparisonArea(*this,
                                           PG_Rect(0,
                                                   top,
                                                   this->m_window->Width(),
                                                   (int)(text_part * height)),
                                           &this->m_recognition);
                                           
  // Create area for wave, spectrogram and recognition text.
  this->m_recognition_area =
    new WidgetRecognitionArea(this->m_window,
                              PG_Rect(0,
                                      (int)(top + space + text_part * height),
                                      this->m_window->Width(),
                                      (int)(recognizer_part * height)),
                              this->m_audio_input,
                              &this->m_recognition,
                              250);
                              
  // Enable recognizer.
  this->m_pauserecog_button->SetPressed(false);
  this->m_queue.enable();
  if (!this->m_queue.start()) {
    this->error("Couldn't start queue in WindowRecognizer::open.", ERROR_CLOSE);
    return;
  }

  // Do reseting
  this->reset(true);
//  this->set_status(LISTENING);
  this->pause_audio_input(true);
  this->handle_stop_button();
  
  // Hide advanced buttons.
  this->m_showadvanced_button->SetPressed(false);
  this->handle_showadvanced_button();
  
}

void
WindowRecognizer::do_running()
{
  /* A simple way to estimate fps.
  static unsigned long luku = 0;
  luku++;
  if (luku % 1000 == 0)
    fprintf(stderr, "%d\n", luku);
  //*/

  // Check broken pipe flags.
  if (this->m_broken_pipe || this->m_queue.is_broken_pipe()) {
    this->m_queue.stop();
//    this->m_queue.disable();
    this->handle_broken_pipe();
    this->m_queue.start();
  }
  else {
    if (this->m_audio_input->is_eof() && this->m_record_button->IsHidden())
      this->handle_stop_button();
//    unsigned long read_size = this->get_audio_input()->operate();

    this->m_audio_input->operate();
    this->flush_out_queue();
    this->m_recognition_area->update();
  }
  
}

void
WindowRecognizer::do_closing(int return_value)
{
  this->m_queue.stop();
  
  if (this->m_texts_area) {
    this->m_window->RemoveChild(this->m_texts_area);
    delete this->m_texts_area;
    this->m_texts_area = NULL;
  }    

  if (this->m_recognition_area) {
    this->m_window->RemoveChild(this->m_recognition_area);
    delete this->m_recognition_area;
    this->m_recognition_area = NULL;
  }

  if (this->m_audio_input) {
    this->m_audio_input->terminate();
    delete this->m_audio_input;
    this->m_audio_input = NULL;
  }
}

bool
WindowRecognizer::handle_broken_pipe()
{
  int ret_val;

  this->m_broken_pipe = true;  
  if (!this->error("Recognition process disconnected.\nPress OK to restart.",
                   ERROR_NORMAL)) {
    return false;
  }

  WindowStartProcess window(this->m_window, this->m_recognizer);
  window.initialize();
  
  // NOTE: We have to close audio stream or the forked process will have its own
  // copy of this stream open and in that case we couldn't open audio streams
  // anymore. (Don't know if there's a better way to deal with this? fcloseall
  // did not work..)
  this->m_audio_input->terminate();

  ret_val = this->run_child_window(&window);

  // Check that whole application wasn't terminated.  
  if (ret_val != 0) {
    this->reset(false);
    this->m_recognizer->get_out_queue()->clear();
//    this->handle_adapt_button();
    // TODO: this->handle_stop_button(); ???
    this->m_broken_pipe = false;

    // Now re-open the audio stream (look at the note above).
    if (!this->m_audio_input->initialize()) {
      this->error("Failed to re-initialize audio input.", ERROR_CLOSE);
      ret_val = 0;
    }
  }

  return !this->m_broken_pipe;
}
//*
void
WindowRecognizer::flush_out_queue()
{
  try {
    if (this->m_recognizer)
      this->m_recognizer->get_out_queue()->flush();
  }
  catch (msg::ExceptionBrokenPipe exception) {
    this->m_broken_pipe = true;
  }
}
//*/
void
WindowRecognizer::pause_window_functionality(bool pause)
{
  if (pause) {
    Window::pause_window_functionality(true);
    // Pause audio and recognizer.
    this->m_audio_input->pause_listening(true);
    this->enable_recognizer(false);
    // Disable thread reading input pipe (RecognizerListener).
    this->m_queue.disable();
  }
  else {
    // Enable thread reading input pipe (RecognizerListener).
    this->m_queue.enable();
    // Set pausing states according to gui button states.
    this->handle_pauserecog_button();
    this->handle_pause_button();
    Window::pause_window_functionality(false);
  }
}

void
WindowRecognizer::reset(bool reset_audio)
{
  WindowReset window(this->m_window,
                     this->m_recognizer ? this->m_recognizer->get_in_queue() : NULL,
                     this->m_recognizer ? this->m_recognizer->get_out_queue() : NULL);

  // Window functionality will be paused and unpaused when running a child
  // window but we want to do these pauses before running the child window
  // because we are resetting some shared resources.
  this->pause_window_functionality(true);
  this->pause_audio_input(true);

  if (reset_audio)
    this->m_audio_input->reset();
  else
    this->m_audio_input->reset_cursors();

  // Reset recognition before recognition area, because it resets and updates
  // according to recognition.
  this->m_recognition.reset();
  this->m_recognition_area->reset();
  this->m_recognition_area->update();

  window.initialize();
  /*
  if (this->run_child_window(&window) == -1) {
    this->m_broken_pipe = true;
    return;
  }
  //*/
  //* If we don't run reset window, use these lines.
  if (this->m_recognizer) {
    msg::Message message(msg::M_RESET, true);
    this->m_recognizer->get_out_queue()->clear_non_urgent();
    this->m_recognizer->get_out_queue()->add_message(message);
    this->flush_out_queue();
    this->m_queue.wait_for_ready();
  }
  this->pause_window_functionality(false);
  //*/

  // End adaptation.  
  if (this->m_adapt_check->GetPressed()) {
    if (this->m_recognizer) {
      msg::Message message(msg::M_ADAPT_OFF);
      this->m_recognizer->get_out_queue()->add_message(message);
      this->flush_out_queue();
    }
  }

//  this->handle_stop_button();
  this->handle_pauserecog_button();
}

void
WindowRecognizer::pause_audio_input(bool pause)
{
  this->m_pause_button->SetPressed(pause);
  this->handle_pause_button();
}

void
WindowRecognizer::end_of_audio()
{
  this->pause_audio_input(true);
//  this->set_status(END_OF_AUDIO);
  if (this->m_recognizer) {
    msg::Message message(msg::M_AUDIO_END, true);
    this->m_recognizer->get_out_queue()->add_message(message);
    this->flush_out_queue();
  }
}
/*
void
WindowRecognizer::set_status(Status status)
{
  this->m_status = status;
}
//*/
void
WindowRecognizer::enable_recognizer(bool enable)
{
  if (this->m_recognizer) {

    msg::Message message(msg::M_DECODER_PAUSE, true);
    if (enable)
      message.set_type(msg::M_DECODER_UNPAUSE);

    this->m_recognizer->get_out_queue()->add_message(message);
    this->flush_out_queue();
  }
}
/*
bool
WindowRecognizer::handle_play_button()
{
  if (this->m_status == LISTENING) {
    this->pause_audio_input(!this->m_play_button->GetPressed());
  }
  return true;
}
//*/
bool
WindowRecognizer::handle_pauserecog_button()
{
  this->enable_recognizer(!this->m_pauserecog_button->GetPressed());
  return true;
}
/*
bool
WindowRecognizer::handle_endaudio_button()
{
  if (this->m_status == LISTENING) {
    this->end_of_audio();
  }
  return true;
}
//*/
bool
WindowRecognizer::handle_record_button()
{
  this->m_audio_input->set_mode(AudioInputController::RECORD);
  this->reset(true);
  // Show some buttons.
  this->m_pause_button->Show(false);
  this->m_stop1_button->Show(false);
  // Hide some buttons.
  this->m_record_button->Hide(false);
  this->m_recognize_button->Hide(false);
  this->m_stop2_button->Hide(false);

  // Unpause automatically.  
  this->pause_audio_input(false);

  // Start adaptation if requested.
  if (this->m_adapt_check->GetPressed()) {
    if (this->m_recognizer) {
      msg::Message message(msg::M_ADAPT_ON);
      this->m_recognizer->get_out_queue()->add_message(message);
      this->flush_out_queue();
    }
    this->m_adapt_check->EnableReceiver(false);
  }
  return true;
}

bool
WindowRecognizer::handle_recognize_button()
{
  this->m_audio_input->set_mode(AudioInputController::PLAY);
  this->reset(false);
  //Show some buttons.
  this->m_pause_button->Show(false);
  this->m_stop2_button->Show(false);
  // Hide some buttons.
  this->m_record_button->Hide(false);
  this->m_recognize_button->Hide(false);
  this->m_pause_button->Show(false);
  this->m_stop1_button->Hide(false);
  
  // Unpause automatically.
  this->pause_audio_input(false);

  // Start adaptation if requested.
  if (this->m_adapt_check->GetPressed()) {
    if (this->m_recognizer) {
      msg::Message message(msg::M_ADAPT_ON);
      this->m_recognizer->get_out_queue()->add_message(message);
      this->flush_out_queue();
    }
    this->m_adapt_check->EnableReceiver(false);
  }
  return true;
}

bool
WindowRecognizer::handle_pause_button()
{
  this->m_audio_input->pause_listening(this->m_pause_button->GetPressed());
//  this->pause_audio_input(this->m_pause_button->GetPressed());
//  this->m_audio_input->pause_listening(!this->m_audio_input->is_paused());
  return true;
}

bool
WindowRecognizer::handle_stop_button()
{
  // Show some buttons.
  this->m_record_button->Show(false);
  if (this->m_audio_input->get_audio_data_size())
    this->m_recognize_button->Show(false);
  else
    this->m_recognize_button->Hide(false);
  // Hide some buttons.
  this->m_stop1_button->Hide(false);
  this->m_stop2_button->Hide(false);
  this->m_pause_button->Hide(false);
  
  this->end_of_audio();

  // Set Adapt check button state.
  this->m_adapt_check->SetUnpressed();
  this->m_adapt_check->EnableReceiver(true);
  
  return true;
}
/*
bool
WindowRecognizer::handle_reset_button()
{
  this->reset(true);
  return true;
}
//*/
//*
bool
WindowRecognizer::handle_resetrecog_button()
{
  this->m_audio_input->set_mode(AudioInputController::PLAY);
  this->reset(false);
  this->handle_stop_button();
  return true;
}
//*/
bool
WindowRecognizer::handle_settings_button()
{
  WindowSettings window(this->m_window, this->m_recognizer);
  window.initialize();
  if (this->run_child_window(&window) == -1) {
    this->m_broken_pipe = true;
  }
  return true;
}

bool
WindowRecognizer::handle_back_button()
{
  this->end_running(1);
  return true;
}
/*
bool
WindowRecognizer::handle_adapt_button()
{
  msg::Message message(msg::M_ADAPT_ON);//, true);
  
  if (this->m_adapt_button->GetPressed()) {
    message.set_type(msg::M_ADAPT_ON);
//    this->reset(false);
  }
  else {
    message.set_type(msg::M_ADAPT_OFF);
//    if (this->m_status != END_OF_AUDIO)
//      this->end_of_audio();
  }

  if (this->m_recognizer) {
    this->m_recognizer->get_out_queue()->add_message(message);
    this->flush_out_queue();
  }
  
  return true;
}
//*/
/*
bool
WindowRecognizer::handle_adapt_check()
{
  // ParaGUI check button doesn't (for some reason) uncheck when pressed.
  if (this->m_adapt_check->GetPressed())
    this->m_adapt_check->SetUnpressed();
  return true;
}
//*/
bool
WindowRecognizer::handle_resetadaptation_button()
{
//  this->error("Reset adaptation not implemented.", ERROR_NORMAL);
  if (this->m_recognizer) {
    msg::Message message(msg::M_ADAPT_RESET, true);
    this->m_recognizer->get_out_queue()->add_message(message);
    this->flush_out_queue();
  }
  
  return true;
}

bool
WindowRecognizer::handle_open_button()
{
  WindowOpenAudioFile window(this->m_window, this->m_audio_input);
  int ret_val;
  
  this->pause_window_functionality(true);
  window.initialize();
  ret_val = this->run_child_window(&window);

  if (ret_val == 1) {
    this->handle_stop_button();
    this->m_recognition_area->set_scroll_position(0);
    this->m_audio_input->set_mode(AudioInputController::PLAY);
    // Run reset window without reseting audio.
    this->reset(false);
  }
  this->pause_window_functionality(false);
  return true;
}

bool
WindowRecognizer::handle_save_button()
{
  this->pause_window_functionality(true);
  this->pause_audio_input(true);

  if (this->m_audio_input->get_audio_data_size()) {
    WindowSaveAudioFile window(this->m_window, this->m_audio_input);
    window.initialize();
    this->run_child_window(&window);
  }
  else {
    this->error("No audio to save. Record some audio first using Run-button.", ERROR_NORMAL);
  }

  this->pause_window_functionality(false);

  return true;
}

bool
WindowRecognizer::handle_showadvanced_button()
{
  if (this->m_showadvanced_button->GetPressed()) {
    this->m_adapt_check->Show(false);
    this->m_resetadapt_button->Show(false);
    this->m_pauserecog_button->Show(false);
    this->m_reset_button->Show(false);
  }
  else {
    this->m_pauserecog_button->Hide(false);
    this->m_adapt_check->Hide(false);
    this->m_resetadapt_button->Hide(false);
    this->m_reset_button->Hide(false);
  }
  return true;
}

bool
WindowRecognizer::handle_key_event(const SDL_KeyboardEvent *key)
{
  if (key->keysym.sym == SDLK_SPACE) {
    if (key->type == SDL_KEYDOWN && this->m_stop1_button->IsHidden()) {
      this->handle_record_button();
    }
    if (key->type == SDL_KEYUP && !this->m_stop1_button->IsHidden()) {
      this->handle_stop_button();
    }
    return true;
  }
  return false;
}

PG_Rect
WindowRecognizer::calculate_button_rect(unsigned int column_index,
                                        unsigned int row_index)
{
  const int width = 150;
  const int height = column_index == 3 ? 30 : 40;
  const int horizontal_space = 50;
  const int vertical_space = 0;
  const int columns = 4;
  const int left_margin = (this->m_window->Width() - columns * width - (columns - 1) * horizontal_space) / 2;
  const int top_margin = 10;
  
  // Calculate rect.
  return PG_Rect(left_margin + column_index * (horizontal_space + width),
                 top_margin + row_index * (vertical_space + height),
                 width,
                 height);
}


PG_Button*
WindowRecognizer::construct_button(const std::string &label,
                                   unsigned int column_index,
                                   unsigned int row_index,
                                   const SigC::Slot0<bool> &callback)
{
  // Create button and connect it with the window.               
  PG_Rect rect = this->calculate_button_rect(column_index, row_index);
  PG_Button *button = new PG_Button(this->m_window, rect, label.data());
  button->sigClick.connect(callback);

  return button;
}
