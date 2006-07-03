
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
  
//  this->m_spectrum = NULL;
  
  this->m_audio_input = NULL;
//  this->m_in_queue = in_queue;
//  this->m_out_queue = out_queue;
  
//  this->m_thread_created = false;
//  this->m_reset = false;
}

WindowFileRecognizer::~WindowFileRecognizer()
{
  /*
  this->m_window->RemoveChild(&this->m_back_button);
  this->m_window->RemoveChild(&this->m_play_button);
  this->m_window->RemoveChild(&this->m_reset_button);
  this->m_window->RemoveChild(&this->m_open_button);
  //*/
//  delete this->m_back_button;
//  delete this->m_play_button;
//  delete this->m_reset_button;
  delete this->m_open_button;
  
//  delete this->m_spectrum;
  delete this->m_audio_input;
}

void
WindowFileRecognizer::initialize()
{
  WindowRecognizer::initialize();
  
//  this->m_back_button = new PG_Button(this->m_window, PG_Rect(10,100,150,50), "Back to Menu", PG_Button::CANCEL);
//  this->m_play_button = new PG_Button(this->m_window, PG_Rect(200,100,100,50), "Play");
//  this->m_reset_button = new PG_Button(this->m_window, PG_Rect(400,100,100,50), "Reset");
  this->m_open_button = new PG_Button(this->m_window, PG_Rect(10,200,150,50), "Open file");

//  this->m_window->AddChild(this->m_back_button);
//  this->m_window->AddChild(this->m_play_button);
//  this->m_window->AddChild(this->m_reset_button);
  this->m_window->AddChild(this->m_open_button);

//  this->m_back_button->sigClick.connect(slot(*this, &WindowFileRecognizer::button_pressed));
//  this->m_play_button->sigClick.connect(slot(*this, &WindowFileRecognizer::button_pressed));
//  this->m_reset_button->sigClick.connect(slot(*this, &WindowFileRecognizer::button_pressed));
  this->m_open_button->sigClick.connect(slot(*this, &WindowFileRecognizer::button_pressed));
}
/*
void*
WindowFileRecognizer::in_queue_listener(void *user_data)
{
  msg::Message message;
  WindowFileRecognizer *object = (WindowFileRecognizer*)user_data;
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
  //      this->m_in_queue->flush();
      }
    }
  }
  return NULL;
}
//*/
void
WindowFileRecognizer::do_opening()
{
  this->m_audio_input = new AudioFileInputController(this->m_out_queue);
  if (!this->m_audio_input->initialize()) {
    this->error("Audio input controller initialization failed.", ERROR_CLOSE);
    return;
  }
  WindowRecognizer::do_opening();
  /*
  this->pause_audio_input(true);

  this->m_spectrum = new WidgetSpectrum(this->m_window, PG_Rect(100, 300, 500, 200), this->m_audio_input);
  this->m_window->AddChild(this->m_spectrum);

  this->m_audio_input->start_listening();
  
  if (this->m_thread_created) {
    this->error("Can't create thread for in queue: thread already created.", ERROR_CLOSE);
    return;
  }
  this->m_end_thread = false;
  if (pthread_create(&this->m_thread, NULL, this->in_queue_listener, this) != 0) {
    this->error("Couldn't create thread for in queue.", ERROR_CLOSE);
    return;
  }
  this->m_thread_created = true;
  //*/
}

void
WindowFileRecognizer::do_running()
{
  WindowRecognizer::do_running();
  /*
  msg::Message message;
  
  this->m_spectrum->update();

  if (this->m_reset) {
    this->do_reseting();
    this->m_reset = false;
  }
//*/
}

void
WindowFileRecognizer::do_closing()
{
  /*
  msg::Message message;

  this->stop_inqueue_thread();
  this->m_audio_input->stop_listening();

  if (this->m_out_queue) {  
    // tähän out_queue.remove_all tms...
    message.set_type(msg::M_AUDIO_END);
    message.clear_data();
    this->m_out_queue->send_message(message);
  }

  if (this->m_spectrum) {
    this->m_window->RemoveChild(this->m_spectrum);
    delete this->m_spectrum;
    this->m_spectrum = NULL;
  }
  //*/
  WindowRecognizer::do_closing();
  if (this->m_audio_input) {
    this->m_audio_input->terminate();
    delete this->m_audio_input;
    this->m_audio_input = NULL;
  }
}
/*
void
WindowFileRecognizer::do_reseting()
{
  WindowReset window(this->m_in_queue, this->m_out_queue);

  this->pause_audio_input(true);
  this->stop_inqueue_thread();
  this->m_audio_input->stop_listening();
  this->m_audio_input->reset();

  window.initialize();
  this->run_child_window(&window);
  
  this->m_audio_input->start_listening();
  this->start_inqueue_thread();
}
//*/
/*
void
WindowFileRecognizer::pause_audio_input(bool pause)
{
  if (this->m_audio_input) {
    this->m_audio_input->pause_listening(pause);
    if (pause) {
      this->m_play_button->SetText("Play");
    }
    else {
      this->m_play_button->SetText("Pause");
    }        
  }
}
//*/
/*
void
WindowFileRecognizer::reset_audio_input()
{
  this->m_reset = true;
}
//*/
/*
bool
WindowFileRecognizer::start_inqueue_thread()
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
WindowFileRecognizer::stop_inqueue_thread()
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
//*/
bool
WindowFileRecognizer::button_pressed(PG_Button *button)
{
  /*
  if (button == this->m_back_button) {
    this->close(1);
  }
  else if (button == this->m_reset_button) {
    this->reset_audio_input();
  }
  else if (button == this->m_play_button) {
    this->pause_audio_input(!this->m_audio_input->is_paused());
  }
  else//*/
  if (button == this->m_open_button) {
    this->pause_audio_input(true);
    this->m_audio_input->load_file("chunk.wav");
  }

  return true;
}
/*
bool
WindowFileRecognizer::eventKeyDown(const SDL_KeyboardEvent *key)
{
  fprintf(stderr, "WindowFileRecognizer::eventKeyDown\n");
  this->close();

  // Exit on Esc.
  if (key->keysym.sym == SDLK_ESCAPE)
    this->quit();

  return true;
}
//*/
