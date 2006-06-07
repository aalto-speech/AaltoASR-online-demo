
#ifndef AUDIOINPUTCONTROLLER_HH_
#define AUDIOINPUTCONTROLLER_HH_

#include <iostream>
#include <queue>
#include "FileOutput.hh"
#include "AudioInput.hh"

//using namespace std::queue;

class AudioInputController
{
  
public:
  
  AudioInputController();
  ~AudioInputController();

  // Starts a new thread which listens audio in and sends it to file out
  bool start_listening();
  void stop_listening();
  void check_buffer_size()  { AudioInput::check_buffer_size(); };
  
  void add_file(FILE *file)  { this->m_file_output.add_file(file); };
  void remove_file(FILE *file)  { this->m_file_output.remove_file(file); };
  void remove_all_files()  { this->m_file_output.remove_all_files(); };
  
  unsigned long get_sample_rate();
  unsigned int get_bytes_per_sample();
  
private:

  static void* activate_thread(void *data);
  void do_listening();

  bool m_stop;
  unsigned long m_block_size;
  AUDIO_FORMAT *m_buffer;
  FileOutput m_file_output;

  struct Command {
    char symbol;
    void *data;
    unsigned int size;
    unsigned int count;
  };

  std::queue<Command> m_command_queue;
  
  pthread_t m_thread;
  
  
};

#endif /*AUDIOINPUTCONTROLLER_HH_*/
