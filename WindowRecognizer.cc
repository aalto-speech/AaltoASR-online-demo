
#include "Recognition.hh"
#include "WindowRecognizer.hh"
#include "WindowReset.hh"
#include "WindowSettings.hh"
#include "str.hh"

WindowRecognizer::WindowRecognizer(msg::InQueue *in_queue, OutQueueController *out_queue)
//    m_spectrum(this, PG_Rect(100, 300, 500, 200))
{
  this->m_back_button = NULL;
  this->m_play_button = NULL;
  this->m_reset_button = NULL;
  this->m_resetrecog_button = NULL;
  this->m_enablerecog_button = NULL;
  this->m_endaudio_button = NULL;
  this->m_settings_button = NULL;

  this->m_recognition_area = NULL;
  
  this->m_in_queue = in_queue;
  this->m_out_queue = out_queue;
  
  this->m_thread_created = false;
//  this->m_reset_pressed = false;
  this->m_button_event = NONE;
  this->m_paused = false;
  this->m_recognizer_enabled = true;
}

WindowRecognizer::~WindowRecognizer()
{
  delete this->m_back_button;
  delete this->m_play_button;
  delete this->m_reset_button;
  delete this->m_resetrecog_button;
  delete this->m_enablerecog_button;
  delete this->m_endaudio_button;
  delete this->m_settings_button;
  
  delete this->m_recognition_area;
}

void
WindowRecognizer::initialize()
{
  Window::initialize();
  
  this->m_play_button = new PG_Button(this->m_window, PG_Rect(200,100,150,50), "Play");
  this->m_enablerecog_button = new PG_Button(this->m_window, PG_Rect(200,200,150,50), "Enable recognizer");
  this->m_endaudio_button = new PG_Button(this->m_window, PG_Rect(200,300,150,50), "End audio");
  this->m_reset_button = new PG_Button(this->m_window, PG_Rect(400,100,150,50), "Reset");
  this->m_resetrecog_button = new PG_Button(this->m_window, PG_Rect(400,200,150,50), "Reset recognizer");
  this->m_settings_button = new PG_Button(this->m_window, PG_Rect(400,300,150,50), "Settings..");
  this->m_back_button = new PG_Button(this->m_window, PG_Rect(this->m_window->my_width - 200,this->m_window->my_height-80,150,50), "Back to Menu", PG_Button::CANCEL);

  this->m_window->AddChild(this->m_play_button);
  this->m_window->AddChild(this->m_enablerecog_button);
  this->m_window->AddChild(this->m_endaudio_button);
  this->m_window->AddChild(this->m_reset_button);
  this->m_window->AddChild(this->m_resetrecog_button);
  this->m_window->AddChild(this->m_settings_button);
  this->m_window->AddChild(this->m_back_button);

  this->m_play_button->sigClick.connect(slot(*this, &WindowRecognizer::handle_button));
  this->m_enablerecog_button->sigClick.connect(slot(*this, &WindowRecognizer::handle_button));
  this->m_endaudio_button->sigClick.connect(slot(*this, &WindowRecognizer::handle_button));
  this->m_reset_button->sigClick.connect(slot(*this, &WindowRecognizer::handle_button));
  this->m_resetrecog_button->sigClick.connect(slot(*this, &WindowRecognizer::handle_button));
  this->m_settings_button->sigClick.connect(slot(*this, &WindowRecognizer::handle_button));
  this->m_back_button->sigClick.connect(slot(*this, &WindowRecognizer::handle_button));
  
  this->m_enablerecog_button->SetToggle(true);
}

void
WindowRecognizer::parse_recognition(const std::string &message)
{
  std::vector<std::string> split_vector;
  std::string recognition_string;
  std::string hypothesis_string;
//  std::string data;
//  std::string time_string;
  
  str::split(&message, "*", true, &split_vector, 2);
  if (split_vector.size() >= 1)
    recognition_string = split_vector.at(0);
  if (split_vector.size() >= 2)
    hypothesis_string = split_vector.at(1);
  
  split_vector.clear();
  str::split(&recognition_string, " ", true, &split_vector);

  if (split_vector.size() >= 2) {
    Morpheme morph;
    long next_time;
    bool ok = true;
    unsigned int ind = 0;
    morph.time = str::str2long(&split_vector.at(2*ind), &ok);
    morph.data = split_vector.at(2*ind+1);
    for (ind = 1; ind < split_vector.size() / 2; ind++) {
      next_time = str::str2long(&split_vector.at(2*ind), &ok);
      morph.duration = next_time - morph.time;
      this->m_recognition.add_morpheme(morph);
      fprintf(stderr, "PARSED: Morpheme: %s, Time: %d, Duration: %d\n", morph.data.data(), morph.time, morph.duration);
      morph.time = next_time;
      morph.data = split_vector.at(2*ind+1);
    }
    next_time = str::str2long(&hypothesis_string, &ok);
    morph.duration = next_time - morph.time;
    this->m_recognition.add_morpheme(morph);
    fprintf(stderr, "PARSED: Morpheme: %s, Time: %d, Duration: %d\n", morph.data.data(), morph.time, morph.duration);
  }
}

void*
WindowRecognizer::in_queue_listener(void *user_data)
{
  //*
  msg::Message message;
  WindowRecognizer *object = (WindowRecognizer*)user_data;
  while (!object->m_end_thread) {
    // Read input from recognizer.
    if (object->m_in_queue && object->m_recognizer_enabled) {
      object->m_in_queue->flush();
      if (!object->m_in_queue->empty()) {
        message = object->m_in_queue->queue.front();
        if (message.type() == msg::M_RECOG) {
          fprintf(stderr, "RECOGNITION: %s\n", message.data_ptr());
          object->parse_recognition(message.buf.substr(msg::header_size));
        }
        if (message.type() == msg::M_MESSAGE) {
          fprintf(stderr, "MESSAGE: %s\n", message.data_ptr());
        }
        object->m_in_queue->queue.pop_front();
      }
      else {
        pthread_yield();
      }
    }
    // Flush output to recognizer.
    if (object->m_out_queue) {
      object->m_out_queue->flush();
    }
  }
  //*/
  return NULL;
}

void
WindowRecognizer::do_opening()
{
  this->pause_audio_input(true);
  
  // Enable recognizer.
  this->m_enablerecog_button->SetPressed(true);
  this->m_recognizer_enabled = true;

  this->m_recognition_area = new WidgetRecognitionArea(this->m_window,
                                                       PG_Rect(0, 400, this->m_window->Width(), 200),
                                                       this->get_audio_input(),
                                                       &this->m_recognition,
                                                       250);
  this->m_window->AddChild(this->m_recognition_area);

  this->start_inqueue_thread();

  // Do reseting
  this->m_button_event = RESET;
//  this->m_reset_pressed = true;
  this->set_status(LISTENING);
}

void
WindowRecognizer::do_running()
{
  msg::Message message;
  WindowSettings *window;
  
  switch (this->m_button_event) {
  case ENABLE_RECOG:
    this->m_recognizer_enabled = this->m_enablerecog_button->GetPressed();
    break;
  case END_AUDIO:
    this->end_of_audio();
    break;
  case RESET:
    this->pause_window_functionality(true);
    this->reset(true);
    this->pause_window_functionality(false);
    break;
  case RESET_RECOG:
    this->pause_window_functionality(true);
    this->reset(false);
    this->pause_window_functionality(false);
    break;
  case SETTINGS:
    this->pause_window_functionality(true);
    window = new WindowSettings(this->m_window, this->m_out_queue);
    window->initialize();
    this->run_child_window(window);
    delete window;
    this->pause_window_functionality(false);
    break;
  case NONE:
    break;
  }
  this->m_button_event = NONE;

//  if (!this->m_paused) {
    this->get_audio_input()->listen();
//  }

  this->m_recognition_area->update();
}

void
WindowRecognizer::do_closing(int return_value)
{
  msg::Message message;

  this->stop_inqueue_thread();

  if (this->m_out_queue) {
    this->m_out_queue->reset();
  }
  
  if (this->m_recognition_area) {
    this->m_window->RemoveChild(this->m_recognition_area);
    delete this->m_recognition_area;
    this->m_recognition_area = NULL;
  }
}

void
WindowRecognizer::pause_window_functionality(bool pause)
{
  if (pause) {
    this->pause_audio_input(true);
    this->stop_inqueue_thread();
  }
  else {
    this->start_inqueue_thread();
  }
}

void
WindowRecognizer::reset(bool reset_audio)
{
  WindowReset window(this->m_window, this->m_in_queue, this->m_out_queue);

  if (reset_audio)
    this->get_audio_input()->reset();
  else
    this->get_audio_input()->reset_cursors();

  this->m_recognition.reset();
  this->m_recognition_area->reset();
  this->m_recognition_area->update();

  window.initialize();
  this->run_child_window(&window);
}

void
WindowRecognizer::pause_audio_input(bool pause)
{
//  if (this->m_audio_input) {
    this->get_audio_input()->pause_listening(pause);
    if (pause) {
      this->m_play_button->SetText("Play");
    }
    else {
      this->m_play_button->SetText("Pause");
    }
    
    if (this->m_recognition_area)
      this->m_recognition_area->set_autoscroll(!pause);
      
    this->m_paused = pause;
//  }
}

void
WindowRecognizer::end_of_audio()
{
  this->pause_audio_input(true);
  this->set_status(END_OF_AUDIO);
  if (this->m_recognition_area)
    this->m_recognition_area->set_autoscroll(false);
  if (this->m_out_queue)
    this->m_out_queue->send_message(msg::Message(msg::M_AUDIO_END));
}

void
WindowRecognizer::set_status(Status status)
{
  this->m_status = status;
  switch (status) {
  case END_OF_AUDIO:
    this->m_play_button->Hide(false);
    this->m_endaudio_button->Hide(false);
    break;
  case LISTENING:
    this->m_play_button->Show(false);
    this->m_endaudio_button->Show(false);
    break;
  default:
    fprintf(stderr, "WindowRecognizer::set_status unknown status.\n");
  }
}

bool
WindowRecognizer::start_inqueue_thread()
{
  if (this->m_thread_created) {
    this->error("Can't create thread for in queue: thread already created.", ERROR_CLOSE);
    return false;
  }
  this->m_end_thread = false;
  if (pthread_create(&this->m_thread, NULL, this->in_queue_listener, this) != 0) {
    this->error("Couldn't create thread for in queue.", ERROR_CLOSE);
    return false;
  }
  this->m_thread_created = true;
  return true;
}

void
WindowRecognizer::stop_inqueue_thread()
{
  if (this->m_thread_created) {
    this->m_end_thread = true;
    pthread_join(this->m_thread, NULL);
    this->m_thread_created = false;
  }
  else {
    fprintf(stderr, "Warning: Trying to join thread that is not created "
                    "in WFR::stop_inqueue_thread.\n");
  }
}

bool
WindowRecognizer::handle_button(PG_Button *button)
{
  if (button == this->m_play_button) {
    this->pause_audio_input(!this->m_paused);
  }
  else if (button == this->m_enablerecog_button) {
    this->m_button_event = ENABLE_RECOG;
  }
  else if (button == this->m_endaudio_button) {
    this->m_button_event = END_AUDIO;
  }
  else if (button == this->m_reset_button) {
    this->m_button_event = RESET;
  }
  else if (button == this->m_resetrecog_button) {
    this->m_button_event = RESET_RECOG;
  }
  else if (button == this->m_settings_button) {
    this->m_button_event = SETTINGS;
  }
  else if (button == this->m_back_button) {
    this->close(1);
  }

  return true;
}
