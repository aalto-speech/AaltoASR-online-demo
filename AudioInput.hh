
#ifndef AUDIOINPUT_HH_
#define AUDIOINPUT_HH_

#include <pthread.h>
// It would be cleaner if these inclusions were in the cc file
#include "AudioStream.hh"
#include "Buffer.hh"

/*
 * Kommentteja:
 * - jos bufferista voi lukea aina vain tietyn kokoisen p‰tk‰n, niin sinne
 * j‰‰ lopuksi pieni p‰tk‰..
 */

// NOTICE THIS DEFINITION!
#define AudioBuffer Buffer<AUDIO_FORMAT>

/**
 * Don't take instances, just use the static functions.
 * What would be a better way to implement this kind of static class?
 * namespace? c-functions?
 */
class AudioInput
{

public:

  static bool initialize();
  static void terminate();
  static bool start_input();
  //pause_input? ja stop_input tekee closen...??
  static void stop_input();
  static bool read_block(AUDIO_FORMAT *to, unsigned long block_size);
  static bool check_buffer_size();
  static AudioBuffer* get_buffer() { return m_audio_buffer; }

private:

  static bool input_handler(const short *inputBuffer, short *outputBuffer,
                            unsigned long frameCount, void *userData);
  static AudioStream *m_audio_stream;                            
  static AudioBuffer *m_audio_buffer;
//  static unsigned int m_buffer_lock;
  static pthread_mutex_t m_lock;
  static unsigned long m_buffer_min_size;
  
  // unsigned long buffer_min_size, bool dynamic_buffer

};

#endif
