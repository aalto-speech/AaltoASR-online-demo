
#include <sndfile.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "AudioStream.hh"
  
  
//*  
bool
audio::read_wav_data(const std::string &filename, std::string &to)
{
  SF_INFO info;
  SNDFILE *file;
  char *data;
  unsigned long read_size = 0;

  info.format = 0;
  file = sf_open(filename.data(), SFM_READ, &info);
  
  if (file == NULL) {
    perror("sf_open failed");
    fprintf(stderr, "read_wav_data failed to open file \"%s\"\n", filename.data());
    return false;
  }
    
  data = new char[sizeof(AUDIO_FORMAT)*info.frames+1];
  read_size = audio_read_function(file, (AUDIO_FORMAT*)data, info.frames);
  // Because there might be '\0' character in the data, let's not use =-operator.
  to.assign(data, read_size * sizeof(AUDIO_FORMAT));

  delete [] data;
  sf_close(file);
  return true;
}

bool
audio::write_wav_data(const std::string &filename, const AUDIO_FORMAT *from, unsigned long frames)
{
  SF_INFO info;
  SNDFILE *file;

  info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
  info.samplerate = SAMPLE_RATE;
  info.channels = 1;
  file = sf_open(filename.data(), SFM_WRITE, &info);
  
  if (file == NULL) {
    fprintf(stderr, "write_wav_data failed to write file \"%s\"\n", filename.data());
    return false;
  }
  
  // Have to use const_cast because libsndfile authors haven't made proper
  // function definition.
  audio_write_function(file, const_cast<AUDIO_FORMAT*>(from), frames);
    
  sf_close(file);
  
  return true;
}

//*/
AudioStream::AudioStream()
{
  this->m_stream = NULL;
  this->m_input_stream = false;
  this->m_output_stream = false;
  this->m_input_buffer = NULL;
  this->m_output_buffer = NULL;
  pthread_mutex_init(&this->m_inputbuffer_lock, NULL);
  pthread_mutex_init(&this->m_outputbuffer_lock, NULL);
//  this->m_frames_played = 0;
}

AudioStream::~AudioStream()
{
  this->close();
  pthread_mutex_destroy(&this->m_inputbuffer_lock);
  pthread_mutex_destroy(&this->m_outputbuffer_lock);
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
AudioStream::open(bool input_stream, bool output_stream)
{
//  this->m_frames_played = 0;
  this->m_input_stream = input_stream;
  this->m_output_stream = output_stream;
  
  PaStreamParameters *input_params = NULL;
  PaStreamParameters *output_params = NULL;
  
  if (input_stream) {
    input_params = new PaStreamParameters;
    input_params->device = 0;
    input_params->channelCount = 1;
    input_params->sampleFormat = PA_AUDIO_FORMAT;
    input_params->suggestedLatency = 0.001;
    input_params->hostApiSpecificStreamInfo = NULL;
  }

  if (output_stream) {
    output_params = new PaStreamParameters;
    output_params->device = 0;
    output_params->channelCount = 1;
    output_params->sampleFormat = PA_AUDIO_FORMAT;
    output_params->suggestedLatency = 0.001;
    output_params->hostApiSpecificStreamInfo = NULL;
  }

  // TODO: Tutki Audacitya miten viiveet on siinä poistettu!!!
  PaError error = Pa_OpenStream(&this->m_stream,
        input_params,// input params
        output_params,// output params
        SAMPLE_RATE,
        128,//paFramesPerBufferUnspecified,
        paNoFlag,//PaStreamFlags streamFlags,
        AudioStream::callback,
        this); // Open audio stream.
        
  delete input_params;
  delete output_params;

/*
  PaError error = Pa_OpenStream(&this->m_stream,
        0,//PaDeviceID inputDevice,
        input_stream ? 1 : 0,
        PA_AUDIO_FORMAT,
        NULL,//void *inputDriverInfo,
        0,//PaDeviceID outputDevice,
        output_stream ? 1 : 0,
        PA_AUDIO_FORMAT,
        NULL,//void *outputDriverInfo,
        SAMPLE_RATE,
        128,//paFramesPerBufferUnspecified,
        0,//numBuffers
        paNoFlag,//PaStreamFlags streamFlags,
        AudioStream::callback,
        this); // Open audio stream.
        //*/
        /*
  PaError error = Pa_OpenDefaultStream(&this->m_stream,
                                       input_stream ? 1 : 0,
                                       output_stream ? 1 : 0,
                                       PA_AUDIO_FORMAT,
                                       SAMPLE_RATE,
                                       paFramesPerBufferUnspecified,
                                       AudioStream::callback,
                                       this);
//*/
  if (error != paNoError) {
    print_error("Couldn't open audio stream:\n", &error);
    return false;
  }
  
  return true;
}

/*
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
//*/
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
/*
void
AudioStream::reset()
{
//  this->m_frames_played = 0;
}
//*/
bool
AudioStream::start()
{
  if (!this->m_stream) {
    print_error("Couldn't start audio stream:\nStream not opened.\n");
    return false;
  }

  //Start stream
  PaError error = Pa_StartStream(this->m_stream);
  if (error != paNoError) {
    this->print_error("Couldn't start audio stream:\n", &error);
    return false;
  }
  return true;
}

void
AudioStream::set_input_buffer(AudioBuffer *input_buffer)
{
  pthread_mutex_lock(&this->m_inputbuffer_lock);
  this->m_input_buffer = input_buffer;
  pthread_mutex_unlock(&this->m_inputbuffer_lock);
}

void
AudioStream::set_output_buffer(AudioBuffer *output_buffer)
{
  pthread_mutex_lock(&this->m_outputbuffer_lock);
  this->m_output_buffer = output_buffer;
  pthread_mutex_unlock(&this->m_outputbuffer_lock);
}


//Static callback function.
int
AudioStream::callback(const void* input_buffer, void* output_buffer,
                      unsigned long frame_count,
                      const PaStreamCallbackTimeInfo *time_info,
                      PaStreamCallbackFlags status_flags, void* instance)
{
  AudioStream *object = (AudioStream*)instance;
  if (object->m_input_stream) {
    object->input_stream_callback((AUDIO_FORMAT*)input_buffer,
                                  frame_count);
  }
  if (object->m_output_stream) {
    object->output_stream_callback((AUDIO_FORMAT*)output_buffer,
                                   frame_count);
  }
  return paContinue;
  /*
  AudioStream *object = (AudioStream*)instance;
  if (!object->stream_callback((AUDIO_FORMAT*)input_buffer,
                               (AUDIO_FORMAT*)output_buffer,
                               frame_count)) {
    return paComplete;
  }
//  luku = frame_count;
//  pthread_yield();
  return paContinue;
  //*/
}

void
AudioStream::input_stream_callback(const AUDIO_FORMAT *input_buffer,
                                   unsigned long frame_count)
{
//  fprintf(stderr, "AS input callback start\n");
  unsigned long write_size;

  pthread_mutex_lock(&this->m_inputbuffer_lock);
  if (this->m_input_buffer) {
    write_size = this->m_input_buffer->write(input_buffer, frame_count);
  
    if (write_size < frame_count) {
      fprintf(stderr, "Warning: Audio input stream buffer full, losing audio.\n");
      assert(false);
    }
  }
  pthread_mutex_unlock(&this->m_inputbuffer_lock);
}

void
AudioStream::output_stream_callback(AUDIO_FORMAT *output_buffer,
                                    unsigned long frame_count)
{
  unsigned long read_size = 0;

  pthread_mutex_lock(&this->m_outputbuffer_lock);
  if (this->m_output_buffer) {
    read_size = this->m_output_buffer->read(output_buffer, frame_count);
  }
  else {
    read_size = 0;
  }
  pthread_mutex_unlock(&this->m_outputbuffer_lock);

  // Prevent undefined scratching output..  
  if (frame_count > read_size) {
    memset(output_buffer + read_size, 0, sizeof(AUDIO_FORMAT) * (frame_count - read_size));
  }

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

