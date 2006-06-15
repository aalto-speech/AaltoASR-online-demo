
#include "AudioInputController.hh"

AudioInputController::AudioInputController(msg::OutQueue *out_queue)
  : m_out_queue(out_queue)
{
  this->m_read_cursor = 0;
  this->m_paused = false;
  this->m_audio_data.clear();
}

AudioInputController::~AudioInputController()
{
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

bool
AudioInputController::start_listening()
{
  this->m_stop = false;
  if (pthread_create(&this->m_thread, NULL, activate_thread, this) != 0) {
    fprintf(stderr, "AIC:start_listening couldn't create new thread.\n");
    return false;
  }
  return true;
}

void
AudioInputController::stop_listening()
{
  this->m_stop = true;

  // Wait for the thread to close. Must be called to avoid memory leaks!
  pthread_join(this->m_thread, NULL);
}
//*
void
AudioInputController::pause_listening(bool pause)
{
  this->m_paused = pause;
}
//*/
//private static
void*
AudioInputController::activate_thread(void *data)
{
  fprintf(stderr, "AIC: new thread started.\n");
  ((AudioInputController*)data)->do_listening();
  fprintf(stderr, "AIC: thread exited normally.\n");
  return NULL;
}

//*
//private
void
AudioInputController::do_listening()
{
  msg::Message message(msg::M_AUDIO);
  const char *audio_data;
  unsigned long read_size, send_size;

  this->m_read_cursor = 0;

  while (!this->m_stop) {
//*
    if (!this->m_paused) {
      read_size = this->read_input();
      read_size *= sizeof(AUDIO_FORMAT);
      
      // Send audio in max 1000 byte messages.
      // Following adding is done because read_size is unsigned so we cannot
      // check read_size < 0
      read_size += 1000;
      while (read_size > 1000) {
        // Calculate the size of data of next message.
        read_size -= 1000;
        if (read_size > 1000)
          send_size = 1000;
        else
          send_size = read_size;
          
        // Clear previous audio data and make message of the new data.
        message.buf.erase(msg::header_size);
        // Write new data.
        audio_data = this->m_audio_data.data();
        message.append(&audio_data[this->m_read_cursor],
                       send_size);
  
        this->m_read_cursor += send_size;
        
        // Send message to out queue. This may block, and it's ok.
        this->m_out_queue->queue.push_back(message); // oma lukko
        this->m_out_queue->flush(); // t‰m‰ t‰‰lt‰ threadista pois
      }
    }
    else {
      // Give other threads some time if this one is paused.
      pthread_yield();
    }
//*/
  }
}
//*/

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
