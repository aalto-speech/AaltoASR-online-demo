
#include "WindowRecognizer.hh"
#include "WindowReset.hh"

WindowRecognizer::WindowRecognizer(msg::InQueue *in_queue, OutQueueController *out_queue)
//    m_spectrum(this, PG_Rect(100, 300, 500, 200))
{
  this->m_back_button = NULL;
  this->m_play_button = NULL;
  this->m_reset_button = NULL;
//  this->m_open_button = NULL;
  
  this->m_spectrum = NULL;
  this->m_recognition_area = NULL;
  
//  this->m_audio_input = NULL;
  this->m_in_queue = in_queue;
  this->m_out_queue = out_queue;
  
  this->m_thread_created = false;
  this->m_reset = false;
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
  
  delete this->m_spectrum;
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

void*
WindowRecognizer::in_queue_listener(void *user_data)
{
  //*
  msg::Message message;
  WindowRecognizer *object = (WindowRecognizer*)user_data;
  while (!object->m_end_thread) {
    if (object->m_in_queue) {
      object->m_in_queue->flush();
      if (!object->m_in_queue->empty()) {
        message = object->m_in_queue->queue.front();
        if (message.type() == msg::M_RECOG) {
          fprintf(stderr, "RECOGNITION: %s\n", message.data_ptr());
        }
        if (message.type() == msg::M_FRAME) {
          fprintf(stderr, "FRAME: %s\n", message.data_ptr());
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
  /*
  this->m_audio_input = new AudioFileInputController(this->m_out_queue);
  if (!this->get_audio_input()->initialize()) {
    this->error("Audio input controller initialization failed.", ERROR_CLOSE);
    return;
  }
  //*/
  /*
  if (!this->m_audio_input->load_file("chunk.wav")) {
    this->error("Couldn't load audio file.", ERROR_CLOSE);
    return;
  }
  //*/
  this->pause_audio_input(true);

//  this->m_recognition_area = new WidgetScrollArea(this->m_window,
//                                                  PG_Rect(0, 400, this->m_window->Width(), 200));
  this->m_spectrum = new WidgetSpectrum(this->m_window, PG_Rect(0, 300, this->m_window->Width(), 100),
                                        this->get_audio_input(), 250);
  this->m_window->AddChild(this->m_recognition_area);
  this->m_window->AddChild(this->m_spectrum);

  this->get_audio_input()->start_listening();
  
  this->start_inqueue_thread();

  this->m_reset = true;
}

void
WindowRecognizer::do_running()
{
  msg::Message message;
  
  this->get_audio_input()->listen();
  this->m_spectrum->update();

  if (this->m_reset) {
    this->do_reseting();
    this->m_reset = false;
  }
//  pthread_yield();
/*
//  msg::Message message;
//  WindowRecognizer *object = (WindowRecognizer*)user_data;
    if (this->m_in_queue) {
      this->m_in_queue->flush();
      if (!this->m_in_queue->empty()) {
        message = this->m_in_queue->queue.front();
        if (message.type() == msg::M_RECOG) {
          fprintf(stderr, "RECOGNITION: %s\n", message.data_ptr());
        }
        if (message.type() == msg::M_FRAME) {
          fprintf(stderr, "FRAME: %s\n", message.data_ptr());
        }
        if (message.type() == msg::M_MESSAGE) {
          fprintf(stderr, "MESSAGE: %s\n", message.data_ptr());
        }
        this->m_in_queue->queue.pop_front();
      }
  }
  //*/
}

void
WindowRecognizer::do_closing()
{
  msg::Message message;

  this->stop_inqueue_thread();
 /*
  if (this->m_thread_created) {
    this->m_end_thread = true;
    pthread_join(this->m_thread, NULL);
    this->m_thread_created = false;
  }
  //*/
  this->get_audio_input()->stop_listening();

  if (this->m_out_queue) {  
    // tähän out_queue.remove_all tms...
    message.set_type(msg::M_AUDIO_END);
    message.clear_data();
    this->m_out_queue->send_message(message);
  }
  
  if (this->m_recognition_area) {
    this->m_window->RemoveChild(this->m_recognition_area);
    delete this->m_recognition_area;
    this->m_recognition_area = NULL;
  }
  if (this->m_spectrum) {
    this->m_window->RemoveChild(this->m_spectrum);
    delete this->m_spectrum;
    this->m_spectrum = NULL;
  }
  /*
  if (this->m_audio_input) {
    this->m_audio_input->terminate();
    delete this->m_audio_input;
    this->m_audio_input = NULL;
  }
  //*/
}

void
WindowRecognizer::do_reseting()
{
  WindowReset window(this->m_in_queue, this->m_out_queue);

  this->pause_audio_input(true);
  this->stop_inqueue_thread();
  this->get_audio_input()->stop_listening();
  this->get_audio_input()->reset();
  this->m_spectrum->reset();

  window.initialize();
  this->run_child_window(&window);
  
  this->get_audio_input()->start_listening();
  this->start_inqueue_thread();
  /*
  msg::Message message;
  bool ready = false;
  PG_Window reset_window(this, PG_Rect(10,10,100,100), "Reseting..", PG_Window::MODAL);
  this->m_window->AddChild(&reset_window);
  
  this->pause_audio_input(true);
  this->stop_inqueue_thread();
  this->m_audio_input->stop_listening();

  this->m_audio_input->reset();
  if (this->m_out_queue) {
    this->m_out_queue->reset();
  }
  fprintf(stderr, "Waiting recognizer to reset...\n");
  reset_window.Show(false);
  if (this->m_in_queue) {
    while (!ready) {
      this->m_in_queue->flush();
      if (!this->m_in_queue->empty()) {
        message = this->m_in_queue->queue.front();
        ready = message.type() == msg::M_READY;
        this->m_in_queue->queue.pop_front();
      }
    }
  }
  reset_window.Hide(false);
  this->m_window->RemoveChild(&reset_window);
  fprintf(stderr, "Recognizer ready.\n");

  this->m_audio_input->start_listening();
  this->start_inqueue_thread();
  //*/
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
//  }
}

void
WindowRecognizer::reset_audio_input()
{
  this->m_reset = true;
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
    this->reset_audio_input();
  }
  else if (button == this->m_play_button) {
    this->pause_audio_input(!this->get_audio_input()->is_paused());
  }
  /*
  else if (button == this->m_open_button) {
    this->pause_audio_input(true);
    this->m_audio_input->load_file("chunk.wav");
  }
  //*/

  return true;
}
/*
bool
WindowRecognizer::eventKeyDown(const SDL_KeyboardEvent *key)
{
  fprintf(stderr, "WindowFileRecognizer::eventKeyDown\n");
  this->close();

  // Exit on Esc.
  if (key->keysym.sym == SDLK_ESCAPE)
    this->quit();

  return true;
}
//*/
