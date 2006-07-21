
#include "QueueController.hh"
#include "str.hh"

QueueController::QueueController(msg::InQueue *in_queue,
                                 Recognition *recognition)
{
  this->m_in_queue = in_queue;
  this->m_recognition = recognition;
  this->m_stop = false;
  this->m_enabled = true;
  this->m_thread_created = false;
  pthread_mutex_init(&this->m_disable_lock, NULL);
}

QueueController::~QueueController()
{
  pthread_mutex_destroy(&this->m_disable_lock);
}

bool
QueueController::start()
{
  this->m_broken_pipe = false;
  if (this->m_thread_created) {
    fprintf(stderr, "Can't create thread for queue: thread already created.");
    return false;
  }
//  this->m_queue.enable(this->m_enablerecog_button->GetPressed());
//  this->m_queue.start();
  this->m_stop = false;

  if (pthread_create(&this->m_thread, NULL, QueueController::callback, this) != 0) {
    fprintf(stderr, "Couldn't create thread for queue.");
    return false;
  }
  
  this->m_thread_created = true;
  return true;
}

void
QueueController::stop()
{
//  if (this->m_thread_created) {
    this->m_stop = true;
    pthread_join(this->m_thread, NULL);
    this->m_thread_created = false;
//  }
  if (!this->m_thread_created) {
    fprintf(stderr, "Warning: Trying to join thread that is not created "
                    "in QC::stop.\n");
  }
  //*/
}

void
QueueController::enable()
{
  this->m_enabled = true;
}

void
QueueController::disable()
{
  pthread_mutex_lock(&this->m_disable_lock);
  this->m_enabled = false;
  pthread_mutex_unlock(&this->m_disable_lock);
}

void*
QueueController::callback(void *user_data)
{
  QueueController *object = (QueueController*)user_data;
  try {
    object->run();
  }
  catch (msg::ExceptionBrokenPipe exception) {
    fprintf(stderr,
            "Exception received from pipe (fd=%d): %s\n",
            exception.get_fd(),
            exception.get_message().data());

    // Just raise a flag, because we want to do proper exception handling in
    // the main thread.
    object->m_broken_pipe = true;
  }
  return NULL;
}

void
QueueController::run()
{
  msg::Message message;
  bool yield = false;
  
  if (!this->m_in_queue)
    return;

  while (!this->m_stop) {
    pthread_mutex_lock(&this->m_disable_lock);
    if (this->m_enabled) {
      if (this->m_in_queue->get_eof()) {
        fprintf(stderr, "Warning: QueueController got eof from input!\n");
        pthread_mutex_unlock(&this->m_disable_lock);
        throw msg::ExceptionBrokenPipe(this->m_in_queue->get_fd());
      }
      // Read input from recognizer.
      this->m_in_queue->flush();
      if (!this->m_in_queue->empty()) {
        message = this->m_in_queue->queue.front();
        if (message.type() == msg::M_RECOG) {
          fprintf(stderr, "RECOGNITION: %s\n", message.data_ptr());
          this->parse_recognition(message.buf.substr(msg::header_size));
        }
        if (message.type() == msg::M_MESSAGE) {
          fprintf(stderr, "MESSAGE: %s\n", message.data_ptr());
        }
        this->m_in_queue->queue.pop_front();
      }
      else {
        yield = true;
      }
    }
    else {
      yield = true;
    }
    pthread_mutex_unlock(&this->m_disable_lock);
    
    if (yield) {
      pthread_yield();
      yield = false;
    }
  }
}

void
QueueController::parse_recognition(const std::string &message)
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
      this->m_recognition->add_morpheme(morph);
      fprintf(stderr, "PARSED: Morpheme: %s, Time: %d, Duration: %d\n", morph.data.data(), morph.time, morph.duration);
      morph.time = next_time;
      morph.data = split_vector.at(2*ind+1);
    }
    if (2 * ind < split_vector.size()) {
      next_time = str::str2long(&split_vector.at(2*ind), &ok);
    }
    else {
      next_time = str::str2long(&hypothesis_string, &ok);
    }
    morph.duration = next_time - morph.time;
    this->m_recognition->add_morpheme(morph);
    fprintf(stderr, "PARSED: Morpheme: %s, Time: %d, Duration: %d\n", morph.data.data(), morph.time, morph.duration);
  }
}
