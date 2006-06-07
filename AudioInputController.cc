
//#include <SDL.h>
//#include <SDL_thread.h>
#include "AudioInput.hh"
#include "AudioInputController.hh"

AudioInputController::AudioInputController()
{
  this->m_block_size = 250;
  this->m_buffer = new AUDIO_FORMAT[this->m_block_size];

  if (!AudioInput::initialize()) {
    fprintf(stderr, "AIC constructor failed: Couldn't initialize AudioInput.\n");
    exit(1);
  }
}

AudioInputController::~AudioInputController()
{
  if (this->m_buffer) {
    delete [] this->m_buffer;
  }
  AudioInput::terminate();
}

bool
AudioInputController::start_listening()
{
  if (!AudioInput::start_input())
    return false;

/*    
  if (!SDL_CreateThread(activate_thread, this)) {
    fprintf(stderr, "AIC::listen couldn't create new thread:\n%s\n", SDL_GetError());
    return false;
  }
  //*/
  if (pthread_create(&this->m_thread, NULL, activate_thread, this) != 0) {
    fprintf(stderr, "AIC:start_listening couldn't create new thread.\n");
    return false;
  }
  return true;
}

void
AudioInputController::stop_listening()
{
  AudioInput::stop_input();
  this->m_stop = true;
}

//private static
void*
AudioInputController::activate_thread(void *data)
{
  ((AudioInputController*)data)->do_listening();
  // mit�h�n ihmett� t�ss� pit�isi palauttaa???
  return NULL;
}

//private
void
AudioInputController::do_listening()
{
  this->m_stop = false;
  while (!this->m_stop) {

    //K�y l�pi komennot
    while (!this->m_command_queue.empty()) {
      // T�ss� pit�isi sitten k�sitell� n�m� komennot
      this->m_command_queue.pop();
    }
    
    // Lue AudioInputin puskurista p�tk�
    while (!this->m_stop && !AudioInput::read_block(this->m_buffer, this->m_block_size))
      ;//SDL_Delay(1); // t�h�n joku idle systeemi...
    
    // L�het� p�tk� file outputiin
    if (!this->m_stop)
      this->m_file_output.write_to_files(this->m_buffer, sizeof(AUDIO_FORMAT), this->m_block_size);
  }
}

unsigned long
AudioInputController::get_sample_rate()
{
  return SAMPLE_RATE;
}

unsigned int
AudioInputController::get_bytes_per_sample()
{
  return sizeof(AUDIO_FORMAT);
}
