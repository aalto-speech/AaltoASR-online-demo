
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
  if (this->m_thread_created) {
    this->m_stop = true;
    pthread_join(this->m_thread, NULL);
    this->m_thread_created = false;
  }
  else {
    fprintf(stderr, "Warning: Trying to join thread that is not created "
                    "in QC::stop.\n");
  }
  //*/
}

void
QueueController::enable()
{
  fprintf(stderr, "QC enable\n");
  this->m_enabled = true;
}

void
QueueController::disable()
{
  fprintf(stderr, "QC disable start\n");
  pthread_mutex_lock(&this->m_disable_lock);
  this->m_enabled = false;
  pthread_mutex_unlock(&this->m_disable_lock);
  fprintf(stderr, "QC disable end\n");
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
//        fprintf(stderr, "QC run not empty\n");
        message = this->m_in_queue->queue.front();
        if (message.type() == msg::M_RECOG) {
          fprintf(stderr, "RECOGNITION: %s\n", message.data_ptr());
          this->parse_message(message.buf.substr(msg::header_size));
        }
        if (message.type() == msg::M_MESSAGE) {
          fprintf(stderr, "MESSAGE: %s\n", message.data_ptr());
        }
        this->m_in_queue->queue.pop_front();
      }
      else {
//        fprintf(stderr, "QC run empty\n");
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
QueueController::parse_message(const std::string &message)
{
  std::vector<std::string> split_vector;
  std::vector<Morpheme> morphemes;
  Morpheme new_morpheme;
  int hypothesis_index = -1;
  bool next_is_time = true;
  bool ok = true;
  long last_time = 0;

  str::split(&message, " ", true, &split_vector);
  
  for (unsigned int ind = 0; ind < split_vector.size(); ind++) {
    if (split_vector.at(ind) == "*") {
      hypothesis_index = ind / 2;
    }
    else {
      if (next_is_time) {
        long time = str::str2long(&split_vector.at(ind), &ok);
        new_morpheme.time = time < 0 ? 0 : time;
        if (morphemes.size() > 0) {
          Morpheme &prev_morph = morphemes.at(morphemes.size()-1);
          prev_morph.duration = time - prev_morph.time;
        }
        next_is_time = false;
        last_time = time;
      }
      else {
        new_morpheme.data = split_vector.at(ind);
        morphemes.push_back(new_morpheme);
        next_is_time = true;
      }
    }
  }
  
  if (!ok) {
    fprintf(stderr, "Warning: QueueController recognition parser did not parse "
                    "time properly.\n");
  }
  
  // Update Recognition object.
  this->m_recognition->lock();
  Hypothesis hypo;
  if (hypothesis_index >= 0) {
    this->m_recognition->add_recognitions(morphemes.begin(),
                                          morphemes.begin() + hypothesis_index);
    hypo.add_morphemes(morphemes.begin() + hypothesis_index, morphemes.end());
  }
  else {
    this->m_recognition->add_recognitions(morphemes.begin(), morphemes.end());
  }
  this->m_recognition->set_hypothesis(hypo);
  this->m_recognition->set_recognition_frame(last_time);
  this->m_recognition->unlock();
  
}
