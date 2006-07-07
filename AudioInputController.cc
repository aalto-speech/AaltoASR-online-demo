
#include "AudioInputController.hh"

AudioInputController::AudioInputController(OutQueueController *out_queue)
  : m_out_queue(out_queue)
{
//  pthread_mutexattr_init(&this->m_lock_attr);
//  pthread_mutexattr_settype(&this->m_lock_attr, PTHREAD_MUTEX_RECURSIVE_NP);
//  pthread_mutex_init(&this->m_lock, &this->m_lock_attr);
  this->m_recognizer_cursor = 0;
//  this->m_thread_created = false;
//  this->m_paused = false;
  this->m_audio_data.clear();
}

AudioInputController::~AudioInputController()
{
//  pthread_mutex_destroy(&this->m_lock);
//  pthread_mutexattr_destroy(&this->m_lock_attr);
}

bool
AudioInputController::initialize()
{
  if (!AudioStream::initialize()) {
    fprintf(stderr, "AIC initialization failed: Audio stream initialization failed\n");
    return false;
  }
  return true;
}

void
AudioInputController::terminate()
{
  AudioStream::terminate();
}
/*
bool
AudioInputController::start_listening()
{
  this->m_stop = false;
  this->m_recognizer_cursor = 0;
  return true;
}
//*/
/*
void
AudioInputController::stop_listening()
{
  this->m_stop = true;
}
//*/
/*
void
AudioInputController::pause_listening(bool pause)
{
  this->m_paused = pause;
}
//*/
//private static
/*
void*
AudioInputController::activate_thread(void *data)
{
  fprintf(stderr, "AIC: new thread started.\n");
  ((AudioInputController*)data)->do_listening();
  fprintf(stderr, "AIC: thread exited normally.\n");
  return NULL;
}
//*/
void
AudioInputController::listen()
{
  static msg::Message message(msg::M_AUDIO);
  static const char *audio_data;
  static unsigned long read_size, send_size;

//  if (!this->m_stop) {
    read_size = this->read_input();
    
    if (this->m_out_queue) {
      // Send audio in max 500 frame messages.
      // Following adding is done because read_size is unsigned so we cannot
      // check read_size < 0
      read_size += 500;
      while (read_size > 500) {
        // Calculate the size of data of next message.
        read_size -= 500;
        if (read_size > 500)
          send_size = 500;
        else
          send_size = read_size;
          
        // Clear previous audio data and make message of the new data.
        message.clear_data();
        // Write new data.
//        if (this->lock_audio_writing()) {
          audio_data = this->m_audio_data.data();
          message.append(&audio_data[this->m_recognizer_cursor*sizeof(AUDIO_FORMAT)],
                         send_size * sizeof(AUDIO_FORMAT));

//          this->unlock_audio_writing();
  
          this->m_recognizer_cursor += send_size;
  
          // Send message to out queue.
          this->m_out_queue->send_message(message);
//        }
      }
    }
    else {
      this->m_recognizer_cursor += read_size;
    }
//  }
}

/*
//private
void
AudioInputController::do_listening()
{
  msg::Message message(msg::M_AUDIO);
  const char *audio_data;
  unsigned long read_size, send_size;

  this->m_read_cursor = 0;

  while (!this->m_stop) {
    read_size = this->read_input();
    
    if (this->m_out_queue) {
      // Send audio in max 500 frame messages.
      // Following adding is done because read_size is unsigned so we cannot
      // check read_size < 0
      read_size += 500;
      while (read_size > 500) {
        // Calculate the size of data of next message.
        read_size -= 500;
        if (read_size > 500)
          send_size = 500;
        else
          send_size = read_size;
          
        // Clear previous audio data and make message of the new data.
        message.clear_data();
        // Write new data.
        if (this->lock_audio_writing()) {
          audio_data = this->m_audio_data.data();
          message.append(&audio_data[this->m_read_cursor*sizeof(AUDIO_FORMAT)],
                         send_size * sizeof(AUDIO_FORMAT));

          this->unlock_audio_writing();
  
          this->m_read_cursor += send_size;
  
          // Send message to out queue.
          this->m_out_queue->send_message(message);
        }
      }
    }
    else {
      this->m_read_cursor += read_size;
    }
    // Give other threads some time if this one is paused.
    pthread_yield();
  }
}
//*/

void
AudioInputController::reset()
{
  this->m_audio_data.clear();
  this->reset_cursors();
}

void
AudioInputController::reset_cursors()
{
  this->m_recognizer_cursor = 0;
}

unsigned long
AudioInputController::get_sample_rate() const
{
  return SAMPLE_RATE;
}

unsigned int
AudioInputController::get_bytes_per_sample() const
{
  return sizeof(AUDIO_FORMAT);
}
