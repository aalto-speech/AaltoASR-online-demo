
#include "RecognizerListener.hh"
//#include "str.hh"


RecognizerListener::RecognizerListener(msg::InQueue *in_queue,
                                       RecognitionParser *recognition)
{
  this->m_in_queue = in_queue;
  this->m_recognition = recognition;
  this->m_stop = false;
  this->m_enabled = true;
  this->m_thread_created = false;
//  this->m_wait_ready = 0;
  this->m_wait_ready = false;
  pthread_mutex_init(&this->m_disable_lock, NULL);
}

RecognizerListener::~RecognizerListener()
{
  pthread_mutex_destroy(&this->m_disable_lock);
}

bool
RecognizerListener::start()
{
  this->m_broken_pipe = false;
  this->m_wait_ready = 0;
  if (this->m_thread_created) {
    fprintf(stderr, "Can't create thread for queue: thread already created.");
    return false;
  }

  this->m_stop = false;

  if (pthread_create(&this->m_thread, NULL, RecognizerListener::callback, this) != 0) {
    fprintf(stderr, "Couldn't create thread for queue.");
    return false;
  }
  
  this->m_thread_created = true;
  return true;
}

void
RecognizerListener::stop()
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
RecognizerListener::enable()
{
  this->m_enabled = true;
}

void
RecognizerListener::disable()
{
  pthread_mutex_lock(&this->m_disable_lock);
  this->m_enabled = false;
  pthread_mutex_unlock(&this->m_disable_lock);
}

void*
RecognizerListener::callback(void *user_data)
{
  RecognizerListener *object = (RecognizerListener*)user_data;
  try {
    object->run();
  }
  catch (msg::ExceptionBrokenPipe exception) {
    fprintf(stderr,
            "Exception received from pipe (fd=%d): %s\n",
            exception.get_fd(),
            exception.what());

    // Just raise a flag, because we want to do proper exception handling in
    // the main thread.
    object->m_broken_pipe = true;
  }
  return NULL;
}

void
RecognizerListener::run() throw(msg::ExceptionBrokenPipe)
{
  msg::Message message;
  bool yield = false;
  
  if (!this->m_in_queue)
    return;

  while (!this->m_stop) {
    pthread_mutex_lock(&this->m_disable_lock);
    if (this->m_enabled) {
      
      // Check for broken pipe.
      if (this->m_in_queue->get_eof()) {
        fprintf(stderr, "Warning: QueueController got eof from input!\n");
        pthread_mutex_unlock(&this->m_disable_lock);
        throw msg::ExceptionBrokenPipe(this->m_in_queue->get_fd());
      }

      // Read input from recognizer.
      this->m_in_queue->flush();
      if (!this->m_in_queue->empty()) {
        message = this->m_in_queue->queue.front();
        // Check ready message.
//        if (message.type() == msg::M_READY && this->m_wait_ready > 0) {
        if (message.type() == msg::M_READY && this->m_wait_ready) {
//          this->m_wait_ready--;
//          fprintf(stderr, "Got ready, waiting for %d readys.\n", this->m_wait_ready);
          this->m_wait_ready = false;
        }
        if (!this->m_wait_ready) {
          // Read recognition message if not waiting for ready.
          if (message.type() == msg::M_RECOG) {
            // Pass recognition message forward.
            fprintf(stderr, "RECOG: %s\n", message.data_str().c_str());
            this->m_recognition->lock();
            this->m_recognition->parse(message.buf.substr(msg::header_size));
            this->m_recognition->unlock();
          }
        }
        this->m_in_queue->queue.pop_front();
      }
      else {
        // No messages, we can yield.
        yield = true;
      }
    }
    else {
      // Disabled, we can yield.
      yield = true;
    }
    pthread_mutex_unlock(&this->m_disable_lock);
    
    if (yield) {
      pthread_yield();
      yield = false;
    }
  }
}
