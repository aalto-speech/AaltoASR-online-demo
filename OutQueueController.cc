
#include "OutQueueController.hh"

OutQueueController::OutQueueController(int fd)
  : m_out_queue(fd)
{
  pthread_mutex_init(&this->m_lock, NULL);
  this->m_sending_message = false;
//  this->m_stop = false;
}

OutQueueController::~OutQueueController()
{
  pthread_mutex_destroy(&this->m_lock);
}
/*
bool
OutQueueController::start_flushing()
{
  this->m_stop = false;
  if (pthread_create(&this->m_thread, NULL, OutQueueController::flush, this) != 0) {
    fprintf(stderr, "Flusher couldn't start new thread.\n");
    return false;
  }
  return true;
}

void
OutQueueController::stop_flushing()
{
  this->m_stop = true;
  pthread_join(this->m_thread, NULL);
}
//*/
void
OutQueueController::send_message(const msg::Message &message)
{
  pthread_mutex_lock(&this->m_lock);
  if (message.urgent()) {
    this->m_out_queue.queue.push_front(message);
  }
  else {
    this->m_out_queue.queue.push_back(message);
  }
  pthread_mutex_unlock(&this->m_lock);
}

void OutQueueController::reset()
{
  msg::Message message(msg::M_RESET);
  if (pthread_mutex_lock(&this->m_lock) == 0) {
    this->m_out_queue.queue.clear();
    this->m_out_queue.queue.push_front(message);
//    message.set_urgent(true);
//    this->send_message(message);
    pthread_mutex_unlock(&this->m_lock);
  }
  else {
    fprintf(stderr, "Couldn't reset OQC, mutex locking failed.\n");
  }
}

void
OutQueueController::flush()
{
  bool is_message = false;
  
  do {
    if (!this->m_sending_message) {
      // Prepare next message in queue.
      pthread_mutex_lock(&this->m_lock);
      is_message = this->m_out_queue.prepare_next();
      pthread_mutex_unlock(&this->m_lock);
    }
      
    // If queue had a message, send it.
    if (is_message || this->m_sending_message) {
      // Try sending until the message has been completely sent.
      this->m_sending_message = !this->m_out_queue.send_next();
    }
  } while (is_message && !this->m_sending_message);
}

/*
void*
OutQueueController::flush(void *user_data)
{
  OutQueueController *object = (OutQueueController*)user_data;
  bool is_message = false;
  
  fprintf(stderr, "OutQueueController started new flushing thread.\n");
  while (!object->m_stop) {
    // Prepare next message in queue.
    pthread_mutex_lock(&object->m_lock);
    is_message = object->m_out_queue.prepare_next();
    pthread_mutex_unlock(&object->m_lock);
    
    // If queue had a message, send it.
    if (is_message) {
      // Try sending until the message has been completely sent.
      while (!object->m_out_queue.send_next() && !object->m_stop)
        pthread_yield();
    }
    else {
      pthread_yield();
    }
  }
  fprintf(stderr, "OutQueueController quits flushing thread.\n");
  return NULL;
}
//*/
