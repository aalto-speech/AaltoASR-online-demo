
#include "Recognition.hh"
#include "WindowRecognizer.hh"
#include "WindowReset.hh"
#include "str.hh"

WindowRecognizer::WindowRecognizer(msg::InQueue *in_queue, OutQueueController *out_queue)
//    m_spectrum(this, PG_Rect(100, 300, 500, 200))
{
  this->m_back_button = NULL;
  this->m_play_button = NULL;
  this->m_reset_button = NULL;

  this->m_recognition_area = NULL;
  
  this->m_in_queue = in_queue;
  this->m_out_queue = out_queue;
  
  this->m_thread_created = false;
  this->m_reset = false;
  this->m_paused = false;
}

WindowRecognizer::~WindowRecognizer()
{
  /*
  this->m_window->RemoveChild(&this->m_back_button);
  this->m_window->RemoveChild(&this->m_play_button);
  this->m_window->RemoveChild(&this->m_reset_button);
  this->m_window->RemoveChild(&this->m_open_button);
  //*/
  delete this->m_back_button;
  delete this->m_play_button;
  delete this->m_reset_button;
//  delete this->m_open_button;
  
//  delete this->m_spectrum;
  delete this->m_recognition_area;
//  delete this->m_audio_input;
}

void
WindowRecognizer::initialize()
{
  Window::initialize();
  
  this->m_back_button = new PG_Button(this->m_window, PG_Rect(10,100,150,50), "Back to Menu", PG_Button::CANCEL);
  this->m_play_button = new PG_Button(this->m_window, PG_Rect(200,100,100,50), "Play");
  this->m_reset_button = new PG_Button(this->m_window, PG_Rect(400,100,100,50), "Reset");
//  this->m_open_button = new PG_Button(this->m_window, PG_Rect(10,200,150,50), "Open file");

  this->m_window->AddChild(this->m_back_button);
  this->m_window->AddChild(this->m_play_button);
  this->m_window->AddChild(this->m_reset_button);
//  this->m_window->AddChild(this->m_open_button);

  this->m_back_button->sigClick.connect(slot(*this, &WindowRecognizer::button_pressed));
  this->m_play_button->sigClick.connect(slot(*this, &WindowRecognizer::button_pressed));
  this->m_reset_button->sigClick.connect(slot(*this, &WindowRecognizer::button_pressed));
//  this->m_open_button->sigClick.connect(slot(*this, &WindowFileRecognizer::button_pressed));
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
    if (object->m_in_queue) {
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

  this->m_recognition_area = new WidgetRecognitionArea(this->m_window,
                                                       PG_Rect(0, 400, this->m_window->Width(), 200),
                                                       this->get_audio_input(),
                                                       &this->m_recognition,
                                                       250);
  this->m_window->AddChild(this->m_recognition_area);

//  this->get_audio_input()->start_listening();
  
  this->start_inqueue_thread();

  this->m_reset = true;
  this->set_status(LISTENING);
}

void
WindowRecognizer::do_running()
{
  msg::Message message;
  
  if (!this->m_paused) {
    this->get_audio_input()->listen();
  }

  this->m_recognition_area->update();

  if (this->m_reset) {
    this->pause_window_functionality(true);
    this->reset(true);
    this->m_reset = false;
    this->pause_window_functionality(false);
  }
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
/*
int
WindowRecognizer::run_child_window(Window *child_window)
{
  this->pause_audio_input(true);
  this->stop_inqueue_thread();
//  this->get_audio_input()->stop_listening();
  this->get_audio_input()->reset();
  this->m_recognition.reset();
  this->m_recognition_area->reset();

  window.initialize();
  this->run_child_window(&window);
  
//  this->get_audio_input()->start_listening();
  this->start_inqueue_thread();
}
//*/

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
/*
void
WindowRecognizer::reset_window_components()
{
  this->m_recognition_area->reset();
  this->m_recognition_area->update();
}
//*/
void
WindowRecognizer::reset(bool reset_audio)
{
  WindowReset window(this->m_in_queue, this->m_out_queue);

//  this->pause_window_functionality(true);

  if (reset_audio)
    this->get_audio_input()->reset();
    
  this->m_recognition.reset();
  this->m_recognition_area->reset();
  this->m_recognition_area->update();

  window.initialize();
  this->run_child_window(&window);
  
//  this->pause_window_functionality(false);
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
    break;
  case LISTENING:
    this->m_play_button->Show(false);
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
WindowRecognizer::button_pressed(PG_Button *button)
{
  if (button == this->m_back_button) {
    this->close(1);
  }
  else if (button == this->m_reset_button) {
    this->m_reset = true;
  }
  else if (button == this->m_play_button) {
    this->pause_audio_input(!this->m_paused);
  }

  return true;
}
