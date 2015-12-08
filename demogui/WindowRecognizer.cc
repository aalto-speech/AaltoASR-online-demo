
#include <pgapplication.h>
#include "WindowRecognizer.hh"
#include "WindowMessageBox.hh"
#include "WindowSettings.hh"
#include "WindowStartProcess.hh"
#include "WindowOpenAudioFile.hh"
#include "WindowSaveAudioFile.hh"
#include "str.hh"

WindowRecognizer::WindowRecognizer(RecognizerProcess *recognizer)
  : m_recog_listener(recognizer ? recognizer->get_in_queue() : NULL,
            &m_recog_status)
{
  m_audio_input = NULL;
  
  // Normal buttons
  m_record_button = NULL;
  m_recognize_button = NULL;
  m_pause_button = NULL;
  m_stop1_button = NULL;
  m_stop2_button = NULL;
  
  // Advanced buttons.
  m_showadvanced_button = NULL;
  m_adapt_button = NULL;
  m_resetadapt_button = NULL;
  m_pauserecog_button = NULL;
  m_reset_button = NULL;

  // Child widget areas.
  m_recognition_area = NULL;
  m_comparison_area = NULL;
  m_status_bar = NULL;
  
  m_recog_proc = recognizer;
  
  m_broken_pipe = false;
  
}

WindowRecognizer::~WindowRecognizer()
{
  if (m_audio_input != NULL)
    delete m_audio_input;
}

void
WindowRecognizer::initialize()
{
  Window::initialize();

  // Construct the buttons.
  m_pauserecog_button =
    construct_button("Pause recognizer", 3, 2, slot(*this, &WindowRecognizer::handle_pauserecog_button));
  m_record_button =
    construct_button("RECORD", 0, 0, slot(*this, &WindowRecognizer::handle_record_button));
  m_recognize_button =
    construct_button("RECOGNIZE", 0, 1, slot(*this, &WindowRecognizer::handle_recognize_button));
  m_pause_button =
    construct_button("PAUSE", 0, 2, slot(*this, &WindowRecognizer::handle_pause_button));
  m_stop1_button =
    construct_button("STOP", 0, 0, slot(*this, &WindowRecognizer::handle_stop_button));
  m_stop2_button =
    construct_button("STOP", 0, 1, slot(*this, &WindowRecognizer::handle_stop_button));
  m_reset_button =
    construct_button("Reset recognizer", 3, 3, slot(*this, &WindowRecognizer::handle_resetrecog_button));
  construct_button("Settings..", 2, 0, slot(*this, &WindowRecognizer::handle_settings_button));
  construct_button("Exit", 1, 2, slot(*this, &WindowRecognizer::handle_exit_button));
  m_adapt_button = construct_button("Adapt speaker", 3, 0, slot(*this, &WindowRecognizer::handle_adaptation_button));
  m_resetadapt_button =
    construct_button("Reset adaptation", 3, 1, slot(*this, &WindowRecognizer::handle_resetadaptation_button));
  construct_button("Load audio", 1, 0, slot(*this, &WindowRecognizer::handle_open_button));
  construct_button("Save audio", 1, 1, slot(*this, &WindowRecognizer::handle_save_button));
  m_showadvanced_button =
    construct_button("Show advanced", 2, 2, slot(*this, &WindowRecognizer::handle_showadvanced_button));

  m_pauserecog_button->SetToggle(true);
  m_pause_button->SetToggle(true);
  m_showadvanced_button->SetToggle(true);
  
  m_window->sigKeyUp.connect(slot(*this, &WindowRecognizer::handle_key_event));
  m_window->sigKeyDown.connect(slot(*this, &WindowRecognizer::handle_key_event));
  
  m_status_bar = new WidgetStatus(m_window,
				  PG_Rect(10,
					  m_window->Height() - 25,
					  m_window->Width() - 20,
					  20),
				  &m_recog_status);
}

void
WindowRecognizer::do_opening()
{
  // Initialize audio input.
  m_audio_input = new AudioInputController(m_recog_proc ? m_recog_proc->get_out_queue() : NULL);
  while (!m_audio_input->initialize()) {
    // Audio input initialization may fail if audio device is already in use.
    m_audio_input->terminate();
    WindowMessageBox message(m_window,
                             "Audio initialization error",
                             "Audio input controller initialization failed. Try closing all other programs.",
                             "Retry",
                             "Exit");
    message.initialize();
    // Show the window and quit the program if Retry was not pressed.
    if (run_child_window(&message) != 1) {
      end_running(0);
      return;
    }
  }

  // These constants are used to construct the following areas.
  const unsigned int top = 140;
  const unsigned int space = 5;
  const unsigned int bottom = 35;
  const unsigned int height = m_window->Height() - (top + space + bottom);
  const float text_part = 0.45;
  const float recognizer_part = 1.0 - text_part;

  // Create area for original and recognized texts.
  m_comparison_area = new WidgetComparisonArea(*this,
					       PG_Rect(10,
						       top,
						       m_window->Width() - 20,
						       (int)(text_part * height)),
					       &m_recog_status);
  
  // Create area for wave, spectrogram and recognition text.
  // ADJUST: You may adjust the last parameter (pixels_per_second) to change the
  // time resolution!
  m_recognition_area =
    new WidgetRecognitionArea(m_window,
                              PG_Rect(0,
                                      (int)(top + space + text_part * height),
                                      m_window->Width(),
                                      (int)(recognizer_part * height)),
                              m_audio_input,
                              &m_recog_status,
                              250);
                              
  // Enable recognizer.
  m_pauserecog_button->SetPressed(false);
  m_recog_listener.enable();
  if (!m_recog_listener.start()) {
    error("Couldn't start queue in WindowRecognizer::open.", ERROR_CLOSE);
    return;
  }

  // Do reseting
  reset(true);
  pause_audio_input(true);
  handle_stop_button();
  
  // Hide advanced buttons.
  m_showadvanced_button->SetPressed(false);
  handle_showadvanced_button();
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
  if (m_broken_pipe || m_recog_listener.is_broken_pipe()) {
    m_recog_listener.stop();
    handle_broken_pipe();
    m_recog_listener.start();
  }
  else {
    if (m_audio_input->is_eof() && m_record_button->IsHidden())
      handle_stop_button();

    m_audio_input->operate();
    flush_out_queue();
    m_recognition_area->update();
    m_status_bar->update();
  }
}

void
WindowRecognizer::do_closing(int return_value)
{
  m_recog_listener.stop();
  
  if (m_comparison_area) {
    m_window->RemoveChild(m_comparison_area);
    delete m_comparison_area;
    m_comparison_area = NULL;
  }    

  if (m_recognition_area) {
    m_window->RemoveChild(m_recognition_area);
    delete m_recognition_area;
    m_recognition_area = NULL;
  }

  if (m_audio_input) {
    m_audio_input->terminate();
    delete m_audio_input;
    m_audio_input = NULL;
  }
}

bool
WindowRecognizer::handle_broken_pipe()
{
  int ret_val;
  
  // This function should be called only if recognizer process exists.
  assert(m_recog_proc);

  // NOTE: We have to close audio stream or the forked process will have its own
  // copy of this stream open and in that case we couldn't open audio streams
  // anymore. (Don't know if there's a better way to deal with this? fcloseall
  // did not work..)
  m_audio_input->terminate();

  m_broken_pipe = true;  
  if (!error("Recognition process disconnected.\nPress OK to restart.",
                   ERROR_NORMAL)) {
    return false;
  }

  WindowStartProcess window(m_window, m_recog_proc);
  window.initialize();
  
  // This returns non-zero if restart was successful.
  ret_val = run_child_window(&window);

  // Check that whole application wasn't terminated.  
  if (ret_val != 0) {
    m_audio_input->set_mode(AudioInputController::PLAY);
    reset(false);
    m_recog_proc->get_out_queue()->clear();
    handle_stop_button();
    m_broken_pipe = false;

    // Now re-open the audio stream (look at the note above).
    if (!m_audio_input->initialize()) {
      error("Failed to re-initialize audio input.", ERROR_CLOSE);
      ret_val = 0;
    }
  }

  return !m_broken_pipe;
}

void
WindowRecognizer::flush_out_queue()
{
  try {
    if (m_recog_proc)
      m_recog_proc->get_out_queue()->flush();
  }
  catch (msg::ExceptionBrokenPipe exception) {
    fprintf(stderr, "WindowRecognizer got broken pipe when flushing.\n");
    // Raise flag so we can handle the exception in the do_running function.
    m_broken_pipe = true;
  }
}

void
WindowRecognizer::pause_window_functionality(bool pause)
{
  if (pause) {
    Window::pause_window_functionality(true);
    // Pause audio and recognizer.
    m_audio_input->set_mute(true);
    m_audio_input->pause_listening(true);
    enable_recognizer(false);
    // Disable thread reading input pipe (RecognizerListener).
    m_recog_listener.disable();
  }
  else {
    // Enable thread reading input pipe (RecognizerListener).
    m_recog_listener.enable();
    // Set pausing states according to gui button states.
    handle_pauserecog_button();
    handle_pause_button();
    m_audio_input->set_mute(false);
    Window::pause_window_functionality(false);
  }
}

void
WindowRecognizer::reset(bool reset_audio)
{
  // Window functionality will be paused and unpaused when running a child
  // window but we want to do these pauses before running the child window
  // because we are resetting some shared resources.
  pause_window_functionality(true);
  pause_audio_input(true);

  if (reset_audio)
    m_audio_input->reset();
  else
    m_audio_input->reset_cursors();

  // Reset recognition before recognition area, because it resets and updates
  // according to recognition.
  m_recog_status.reset();
  m_recognition_area->reset();
  m_recognition_area->update();

  /*
  WindowReset window(m_window,
                     m_recognizer ? m_recognizer->get_in_queue() : NULL,
                     m_recognizer ? m_recognizer->get_out_queue() : NULL);

  window.initialize();
  if (run_child_window(&window) == -1) {
    m_broken_pipe = true;
    return;
  }
  //*/
  //* If we don't run reset window, use these lines.
  if (m_recog_proc) {
    msg::Message message(msg::M_RESET, true);
    m_recog_proc->get_out_queue()->clear_non_urgent();
    m_recog_proc->get_out_queue()->add_message(message);
    flush_out_queue();
    m_recog_listener.wait_for_ready();
  }
  pause_window_functionality(false);
  //*/

  // End adaptation.  
  // JPy: No longer needed in the new adaptation logic.
  // if (m_recog_proc) {
  //   msg::Message message(msg::M_ADAPT_OFF);
  //   m_recog_proc->get_out_queue()->add_message(message);
  //   flush_out_queue();
  // }

  m_recog_status.reset_recognition();
  handle_pauserecog_button();
}

void
WindowRecognizer::pause_audio_input(bool pause)
{
  m_pause_button->SetPressed(pause);
  handle_pause_button();
}

void
WindowRecognizer::end_of_audio()
{
  pause_audio_input(true);
  if (m_recog_proc) {
    msg::Message message(msg::M_AUDIO_END, true);
    m_recog_proc->get_out_queue()->add_message(message);
    flush_out_queue();
  }
}

void
WindowRecognizer::enable_recognizer(bool enable)
{
  if (m_recog_proc) {

    msg::Message message(msg::M_DECODER_PAUSE, true);
    if (enable)
      message.set_type(msg::M_DECODER_UNPAUSE);

    m_recog_proc->get_out_queue()->add_message(message);
    flush_out_queue();
  }
}


bool
WindowRecognizer::handle_pauserecog_button()
{
  enable_recognizer(!m_pauserecog_button->GetPressed());
  return true;
}

bool
WindowRecognizer::handle_record_button()
{
  m_audio_input->set_mode(AudioInputController::RECORD);
  reset(true);
  // Show some buttons.
  m_pause_button->Show(false);
  m_stop1_button->Show(false);
  // Hide some buttons.
  m_record_button->Hide(false);
  m_recognize_button->Hide(false);
  m_stop2_button->Hide(false);

  // Unpause automatically.  
  pause_audio_input(false);

  // Start adaptation if requested.
  // m_adapt_check->EnableReceiver(false);
  // if (m_adapt_check->GetPressed()) {
  //   if (m_recog_proc) {
  //     msg::Message message(msg::M_ADAPT_ON);
  //     m_recog_proc->get_out_queue()->add_message(message);
  //     flush_out_queue();
  //   }
  //   m_recog_status.start_adapting();
  // }
  return true;
}

bool
WindowRecognizer::handle_recognize_button()
{
  m_audio_input->set_mode(AudioInputController::PLAY);
  reset(false);
  //Show some buttons.
  m_pause_button->Show(false);
  m_stop2_button->Show(false);
  // Hide some buttons.
  m_record_button->Hide(false);
  m_recognize_button->Hide(false);
  m_pause_button->Show(false);
  m_stop1_button->Hide(false);
  
  // Unpause automatically.
  pause_audio_input(false);

  // Start adaptation if requested.
  // m_adapt_check->EnableReceiver(false);
  // if (m_adapt_check->GetPressed()) {
  //   if (m_recog_proc) {
  //     msg::Message message(msg::M_ADAPT_ON);
  //     m_recog_proc->get_out_queue()->add_message(message);
  //     flush_out_queue();
  //   }
  //   m_recog_status.start_adapting();
  // }
  return true;
}

bool
WindowRecognizer::handle_pause_button()
{
  m_audio_input->pause_listening(m_pause_button->GetPressed());
  return true;
}

bool
WindowRecognizer::handle_stop_button()
{
  // Show some buttons.
  m_record_button->Show(false);
  if (m_audio_input->get_audio_data_size())
    m_recognize_button->Show(false);
  else
    m_recognize_button->Hide(false);
  // Hide some buttons.
  m_stop1_button->Hide(false);
  m_stop2_button->Hide(false);
  m_pause_button->Hide(false);
  
  end_of_audio();
  
  m_audio_input->stop_playback();

  // Set Adapt check button state.
  // m_adapt_check->SetUnpressed();
  // m_adapt_check->EnableReceiver(true);
  
  return true;
}

bool
WindowRecognizer::handle_resetrecog_button()
{
  m_audio_input->set_mode(AudioInputController::PLAY);
  handle_stop_button();
  reset(false);
  return true;
}

bool
WindowRecognizer::handle_settings_button()
{
  WindowSettings window(m_window,
                        m_recog_proc,
                        m_recognition_area->get_spectrogram());
  window.initialize();
  if (run_child_window(&window) == -1) {
    m_broken_pipe = true;
  }
  return true;
}

bool
WindowRecognizer::handle_exit_button()
{
  end_running(1);
  return true;
}

bool
WindowRecognizer::handle_resetadaptation_button()
{
  if (m_recog_proc) {
    msg::Message message(msg::M_ADAPT_RESET, true);
    m_recog_proc->get_out_queue()->add_message(message);
    flush_out_queue();
  }
  m_recog_status.reset_adaptation();
  
  return true;
}

bool
WindowRecognizer::handle_adaptation_button()
{
  if (m_recog_proc) {
    msg::Message message(msg::M_ADAPT_CALC, true);
    m_recog_proc->get_out_queue()->add_message(message);
    flush_out_queue();
  }
  m_recog_status.start_adapting();
  
  return true;
}


bool
WindowRecognizer::handle_open_button()
{
  WindowOpenAudioFile window(m_window, m_audio_input);
  int ret_val;
  
  pause_window_functionality(true);
  window.initialize();
  ret_val = run_child_window(&window);

  if (ret_val == 1) {
    handle_stop_button();
    m_recognition_area->set_scroll_position(0);
    m_audio_input->set_mode(AudioInputController::PLAY);
    // Run reset window without reseting audio.
    reset(false);
  }
  pause_window_functionality(false);
  return true;
}

bool
WindowRecognizer::handle_save_button()
{
  pause_window_functionality(true);
  pause_audio_input(true);

  if (m_audio_input->get_audio_data_size()) {
    WindowSaveAudioFile window(m_window, m_audio_input);
    window.initialize();
    run_child_window(&window);
  }
  else {
    error("No audio to save. Record some audio first using Run-button.", ERROR_NORMAL);
  }

  pause_window_functionality(false);

  return true;
}

bool
WindowRecognizer::handle_showadvanced_button()
{
  if (m_showadvanced_button->GetPressed()) {
    m_adapt_button->Show(false);
    m_resetadapt_button->Show(false);
    m_pauserecog_button->Show(false);
    m_reset_button->Show(false);
  }
  else {
    m_pauserecog_button->Hide(false);
    m_adapt_button->Hide(false);
    m_resetadapt_button->Hide(false);
    m_reset_button->Hide(false);
  }
  return true;
}

bool
WindowRecognizer::handle_key_event(const SDL_KeyboardEvent *key)
{
  if (key->keysym.sym == SDLK_SPACE) {
    if (key->type == SDL_KEYDOWN && m_stop1_button->IsHidden()) {
      handle_record_button();
    }
    if (key->type == SDL_KEYUP && !m_stop1_button->IsHidden()) {
      handle_stop_button();
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
  const int left_margin = (m_window->Width() - columns * width - (columns - 1) * horizontal_space) / 2;
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
  PG_Rect rect = calculate_button_rect(column_index, row_index);
  PG_Button *button = new PG_Button(m_window, rect, label.data());
  button->sigClick.connect(callback);

  return button;
}
