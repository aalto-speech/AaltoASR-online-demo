
#include <sndfile.h>
#include <string.h>
#include <stdio.h>
#include "AudioStream.hh"
  
  
//*  
bool
audio::read_wav_data(const char *filename, std::string *to)
{
  SF_INFO info;
  SNDFILE *file;
  char *data;
  unsigned long read_size = 0;

  info.format = 0;
  file = sf_open(filename, SFM_READ, &info);
  
  if (file == NULL)
    return false;
    
  data = new char[sizeof(AUDIO_FORMAT)*info.frames+1];
  read_size = audio_read_function(file, (AUDIO_FORMAT*)data, info.frames);
  //datassa saattaa piill� lopetusmerkki, joten ei k�ytet� =-operaattoria.
  to->append(data, read_size * sizeof(AUDIO_FORMAT));

  delete [] data;
  sf_close(file);
  return true;
}
//*/
AudioStream::AudioStream()
{
  this->m_stream = NULL;
}

AudioStream::~AudioStream()
{
}

//static
bool
AudioStream::initialize()
{
  PaError error;
  // if initialized, nothing
  // Initialize PortAudio.
  error = Pa_Initialize();
  if (error != paNoError) {
    print_error("Couldn't initialize audio:\n", &error);
    return false;
  }
  return true;
}

//static
void
AudioStream::terminate()
{
  PaError error;
  error = Pa_Terminate();
  if (error != paNoError) {
    print_error("Couldn't terminate audio:\n", &error);
  }
}

bool
AudioStream::open_stream(unsigned int input_channels, unsigned int output_channels)
{
  // Open audio stream.
  PaError error = Pa_OpenDefaultStream(&this->m_stream,
                                       input_channels,
                                       output_channels,
                                       PA_AUDIO_FORMAT,
                                       SAMPLE_RATE,
                                       paFramesPerBufferUnspecified,
                                       this->callback,
                                       this);

  if (error != paNoError) {
    print_error("Couldn't open audio stream:\n", &error);
    return false;
  }
  
  return true;
}

void
AudioStream::close()
{
  if (this->m_stream) {
    PaError error = Pa_CloseStream(this->m_stream);
    if (error != paNoError) {
      print_error("Couldn't close audio stream:\n", &error);
    }
    this->m_stream = NULL;
  }
}

bool
AudioStream::start()
{
  if (!this->m_stream) {
    print_error("Couldn't start audio stream:\nStream not opened.\n");
    return false;
  }

  //Start stream
  PaError error = Pa_StartStream(this->m_stream);
  if ( error != paNoError ) {
    this->print_error("Couldn't start audio stream:\n", &error);
    return false;
  }
  return true;
}

//Static callback function.
int
AudioStream::callback(const void* input_buffer, void* output_buffer,
                      unsigned long frame_count,
                      const PaStreamCallbackTimeInfo *time_info,
                      PaStreamCallbackFlags status_flags, void* instance)
{
  AudioStream *object = (AudioStream*)instance;
  if (!object->stream_callback((AUDIO_FORMAT*)input_buffer,
                               (AUDIO_FORMAT*)output_buffer,
                               frame_count)) {
    return paComplete;
  }
//  luku = frame_count;
//  pthread_yield();
  return paContinue;
}

//static
void
AudioStream::print_error(const std::string &message, const PaError *error)
{
  fprintf(stderr, message.data());
  if (error) {
    fprintf(stderr, Pa_GetErrorText(*error));
  }
//  ErrorHandler::print_error(message);
//  ErrorHandler::print_error(Pa_GetErrorText(error));
}
