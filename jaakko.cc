
#include <stdio.h>
#include "Application.hh"

int main()
{
  Application app;
  int ret_val = EXIT_SUCCESS;

  if (app.initialize()) {
    app.run();
  }
  else {
    fprintf(stderr, "Application initialization failed.\n");
    ret_val = EXIT_FAILURE;
  }
  app.clean();
  fprintf(stderr, "Clean exit.\n");
  return ret_val;
}









/*


#include <SDL.h>
#include <sndfile.h>
#include "AudioLineInputController.hh"
#include "AudioFileInputController.hh"
#include "Process.hh"
#include "msg.hh"
#include "OutQueueController.hh"

void start_recognizer(Process *proc);
bool initialize_sdl();
void put_little(FILE *file, unsigned int size, unsigned long value);
//void write_wav(const char *filename, const char *data, unsigned long sample_rate, unsigned int bytes_per_sample);
//char* get_wav_audio_data(const char *file_name);
FILE *open_wav(const char *file_name, unsigned long sample_rate, unsigned int bytes_per_sample);
void close_wav(FILE *file);

int main2()
{
  Process *proc = new Process();
//  AudioInputController *aic;
  AudioInputController *aic;
  bool quit = false;
  SDL_Event event;
  FILE *file;
  msg::Message message;
//  msg::OutQueue out_queue;
//  pthread_mutex_t out_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
    
  if (!initialize_sdl()) {
    return EXIT_FAILURE;
  }
// 
  // Starts a new process
  start_recognizer(proc);
  msg::InQueue in_queue(proc->read_fd);
  OutQueueController out_queue(proc->write_fd);
//  msg::OutQueue out_queue(proc->write_fd);

  // Wait for ready message
  while (in_queue.empty())
    in_queue.flush();
  message = in_queue.queue.front();
  if (message.type() == msg::M_READY) {
    fprintf(stderr, "Recognizer ready.\n");
  }
  else {
    fprintf(stderr, "Invalid ready message from recognizer.\n");
    assert(false);
  }
  in_queue.queue.pop_front();
///  
  // Initialize audio input
//  aic = new AudioLineInputController(&out_queue);//, NULL);//&out_queue_mutex);
  aic = new AudioFileInputController("chunk.wav", &out_queue);// NULL);//&out_queue_mutex);
  if (!aic->initialize()) {
    return EXIT_FAILURE;
  }
  
  fprintf(stderr, "Start recording.. Press Enter to stop.\n");
  out_queue.start_flushing();
  aic->start_listening();
  
  // Main loop.
  while (!quit) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN)
        quit = true;
      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LEFT) {
        aic->pause_listening(true);
      }
      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RIGHT)
        aic->pause_listening(false);
    }
//     
    in_queue.flush();
    while (!in_queue.empty()) {
//      fprintf(stderr, "Jaakko got message from recognizer.\n");
      message = in_queue.queue.front();
      if (message.type() == msg::M_RECOG) {
        fprintf(stderr, "RECOGNITION: %s\n", message.data_ptr());
      }
      if (message.type() == msg::M_FRAME) {
        fprintf(stderr, "FRAME: %s\n", message.data_ptr());
      }
      if (message.type() == msg::M_MESSAGE) {
        fprintf(stderr, "MESSAGE: %s\n", message.data_ptr());
      }
      in_queue.queue.pop_front();
//      in_queue.flush();
    }
///
  }
  
  aic->stop_listening();
  fprintf(stderr, "Stopped recording.\n");
  
  // Finish recognizer process.
  out_queue.stop_flushing();
  proc->finish();
  delete proc;

  // Write WAV file.  
  file = open_wav("chunk.wav", aic->get_sample_rate(), aic->get_bytes_per_sample());
  const std::string *audio_data = aic->get_audio_data();
  fwrite(audio_data->c_str(), 1, audio_data->length(), file);
  close_wav(file);
  fprintf(stderr, "chunk.wav written.\n");
  
  // Terminate audio input and data.
  aic->terminate();
  delete aic;
  
  SDL_Quit();
  
  fprintf(stderr, "Everything cleaned. Exit.\n");
  return EXIT_SUCCESS;
}

void start_recognizer(Process *proc)//, msg::InQueue *in_queue, msg::OutQueue *out_queue)
{
  if (proc->create() == 0) {
    int ret = execlp(//"./rec.sh", "script",
    "ssh", "ssh", "itl-cl11", "/home/jluttine/workspace/online-demo/rec.sh",
//                    "./recognizer",
    //./recognizer", "./recognizer",
//                    "--config", "mfcc_p_dd.feaconf",
                    (char*)NULL);
    if (ret < 0) {
      perror("exec() failed");
      exit(1);
    }
    assert(false);
  }
  
  msg::set_non_blocking(proc->read_fd);
//  msg::set_non_blocking(proc.write_fd);
//  in_queue->fd = proc->read_fd;
//  out_queue->fd = proc->write_fd;

}
//*
bool
initialize_sdl()
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "Initialization of SDL failed:\n%s\n", SDL_GetError());
    return false;
  }
  if (SDL_SetVideoMode(100, 100, 0, SDL_ANYFORMAT) == NULL) {
    fprintf(stderr, "Setting video mode failed:\n%s\n", SDL_GetError());
    return false;
  }
  return true;
}
//*/
/*
char* get_wav_audio_data(const char *file_name)
{
  SF_INFO info;
  SNDFILE *file;
  AUDIO_FORMAT *data;
  
//  info.channels = 1;
//  info.samplerate = sample_rate;
  info.format = 0;
  
  file = sf_open(filename, SFM_READ, &info);
  
  data = new AUDIO_FORMAT[info.frames];
  sf_read_short(file, data, info.frames);
  return (char*)data;
}

//*/











void put_little(FILE *file, unsigned int size, unsigned long value)
{
  unsigned int i;
  unsigned long divider = 1;
//  char ch;
  
  for (i = 0; i < size; i++) {
    fprintf(file, "%c", (char)((value/divider)&255));
//    fwrite(&ch, 1, 1, file);
    divider *= 256;
  }
}

FILE *open_wav(const char *file_name, unsigned long sample_rate, unsigned int bytes_per_sample)
{
  FILE *file = fopen(file_name, "wb");
  
  // RIFF header
  fprintf(file, "RIFF");
  fprintf(file, "xxxx"); // = data size + 36 = filesize - 8
  fprintf(file, "WAVE");
  
  // fmt subchunk
  fprintf(file, "fmt ");
  put_little(file, 4, 16); // size of rest of the subchunk
  put_little(file, 2, 1); // 1 = pcm
  put_little(file, 2, 1); // 1 = mono
  put_little(file, 4, sample_rate);
  put_little(file, 4, sample_rate * 1 * bytes_per_sample);
  put_little(file, 2, 1 * bytes_per_sample);
  put_little(file, 2, 8 * bytes_per_sample);
  
  // data subchunk
  fprintf(file, "data");
  fprintf(file, "xxxx"); // = data size = filesize - 44
  // and then follows the actual data

  return file;
}

void close_wav(FILE *file)
{
  unsigned long file_size = ftell(file);

  fseek(file, 4, SEEK_SET);
  put_little(file, 4, file_size - 8);

  fseek(file, 40, SEEK_SET);
  put_little(file, 4, file_size - 44);

  fclose(file);
}
