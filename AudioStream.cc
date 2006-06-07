
#include <stdio.h>

#include "AudioStream.hh"
  
AudioStream::AudioStream()
{
  this->m_stream = NULL;
  this->m_callback_function = NULL;
}

AudioStream::~AudioStream()
{
  // Close the stream and terminate PortAudio.
  if (this->m_stream) {
    Pa_CloseStream(this->m_stream);
    this->m_stream = NULL;
  }
}

//static
bool
AudioStream::initialize()
{
  PaError error;
  // Initialize PortAudio.
  error = Pa_Initialize();
  if (error != paNoError) {
    print_error("Couldn't initialize audio stream:\n", &error);
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
AudioStream::open(bool (*callback_function)(const AUDIO_FORMAT *inputBuffer,
                              				      AUDIO_FORMAT *outputBuffer,
                                            unsigned long frameCount,
                                            void *userData),
            		  void *callback_data)
{
  // Open audio stream.
  PaError error = Pa_OpenDefaultStream( &m_stream, //passes back stream pointer
					1, // input channels
					1, // output channels
					PA_AUDIO_FORMAT,
					SAMPLE_RATE, // sample rate
					paFramesPerBufferUnspecified, // frames per bufffer
					this->callback,
					this );

  this->m_callback_function = callback_function;
  this->m_callback_data = callback_data;

  if (error != paNoError) {
    print_error("Couldn't open audio stream.\n", &error);
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
    print_error("Couldn't start audio stream:\nStream not opened.\n", NULL);
    return false;
  }

  //Start stream
  PaError error = Pa_StartStream(this->m_stream);
  if ( error != paNoError ) {
    print_error("Couldn't start audio stream:\n", &error);
    return false;
  }
  return true;
}

void
AudioStream::stop() {
  PaError error;
  if (this->m_stream) {
    error = Pa_StopStream(this->m_stream);
    if (error != paNoError) {
      print_error("Couldn't stop audio stream.\n", &error);
    }
  }
}

void
AudioStream::abort() {
  PaError error;
  if (this->m_stream) {
    error = Pa_AbortStream(this->m_stream);
    if (error != paNoError) {
      print_error("Couldn't stop audio stream.\n", &error);
    }
  }
}


//Static callback function.
int
AudioStream::callback(const void* inputBuffer, void* outputBuffer,
		                  unsigned long frameCount,
            		      const PaStreamCallbackTimeInfo *timeInfo,
            		      PaStreamCallbackFlags statusFlags, void* instance)
{
  AudioStream *object = (AudioStream*)instance;
  if (object->m_callback_function) {
    if (!object->m_callback_function((AUDIO_FORMAT*)inputBuffer,
				                             (AUDIO_FORMAT*)outputBuffer,
                                     frameCount,
                                     object->m_callback_data)) {
      return paComplete;
    }
  }
  return paContinue;
}

//static
void
AudioStream::print_error(const char *message, const PaError *error)
{
  if (message) 
    fprintf(stderr, message);
  if (error)
    fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(*error));
}
