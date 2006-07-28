#include <sndfile.h>
#include <pthread.h>
#include "msg.hh"
#include "conf.hh"
#include "io.hh"
#include "str.hh"
#include "Process.hh"

conf::Config config;
msg::InQueue rec_in_queue;
msg::OutQueue rec_out_queue;
pthread_t thread;
pthread_cond_t cond_var;
pthread_mutex_t cond_mutex;
bool waiting = false;

static void*
input_thread(void *data)
{
  fprintf(stderr, "gui: input thread created\n");
  while (!rec_in_queue.get_eof()) {
    rec_in_queue.flush();
    while (!rec_in_queue.empty()) {
      msg::Message &message = rec_in_queue.queue.front();

      std::string word(message.data_ptr(), message.data_length());
      if (message.type() == msg::M_RECOG)
        fprintf(stderr, "gui: got RECOG: %s\n", word.c_str());

      else if (message.type() == msg::M_RECOG_END) {
        fprintf(stderr, "gui: got RECOG_END\n");
      }
      
      else if (message.type() == msg::M_READY) {
        fprintf(stderr, "gui: got READY\n");
      }
      
      else if (message.type() == msg::M_MESSAGE) {
        std::string str(message.data_ptr(), message.data_length());
        fprintf(stderr, "gui: decoder says %s\n", str.c_str());
      }
      else {
        fprintf(stderr, "gui: got UNKNOWN message of type %d\n",
                message.type());
      }

      rec_in_queue.queue.pop_front();
    }
  }
  fprintf(stderr, "gui: EOF from recognizer\n");
  return NULL;
}

static void*
output_thread(void *data)
{
  fprintf(stderr, "gui: output thread created\n");
  while (1) {
    pthread_mutex_lock(&cond_mutex);
    while (rec_out_queue.queue.empty()) {
      waiting = true;
      pthread_cond_wait(&cond_var, &cond_mutex);
      waiting = false;
    }
    rec_out_queue.prepare_next();
    pthread_mutex_unlock(&cond_mutex);
    rec_out_queue.send_next();
  }
  return NULL;
}

void
create_threads()
{
  pthread_cond_init(&cond_var, NULL);
  pthread_mutex_init(&cond_mutex, NULL);

  int ret = pthread_create(&thread, NULL, input_thread, NULL);

  if (ret != 0) {
    fprintf(stderr, "pthread_create() failed with code %d\n", ret);
    exit(1);
  }

  ret = pthread_create(&thread, NULL, output_thread, NULL);

  if (ret != 0) {
    fprintf(stderr, "pthread_create() failed with code %d\n", ret);
    exit(1);
  }
}

void
enqueue(const msg::Message &msg)
{
  pthread_mutex_lock(&cond_mutex);
  rec_out_queue.queue.push_back(msg);
  if (waiting) {
    pthread_cond_signal(&cond_var);
  }
  pthread_mutex_unlock(&cond_mutex);
}

SNDFILE*
open_audio(std::string file)
{
  SF_INFO snd_info;
  SNDFILE *snd_file = sf_open(file.c_str(), SFM_READ, &snd_info);
  if (snd_file == NULL) {
    fprintf(stderr, "sf_open(%s) failed\n", file.c_str());
    perror("system error:");
  }
  return snd_file;
}

int
main(int argc, char *argv[])
{
  try {
    config("usage: gui [OPTION...]\n")
      ('h', "help", "", "", "display help")
      ('a', "audio=FILE", "arg must", "", "audio file")
      ('r', "rec=FILE", "arg must", "", "rec script file")
      ('\0', "host=STR", "arg must", "", "remote host")
      ;

    config.default_parse(argc, argv);
    if (config.arguments.size() != 0)
      config.print_help(stderr, 1);

    SNDFILE *snd_file = open_audio(config["audio"].get_str());
    
    Process proc;
    if (proc.create() == 0) {
      int ret = execlp("ssh", "ssh", config["host"].get_c_str(), 
                       config["rec"].get_c_str(), NULL);
      if (ret < 0) {
        perror("exec() failed");
        exit(1);
      }
      assert(false);
    }

    rec_in_queue.enable(proc.read_fd);
    rec_out_queue.enable(proc.write_fd);
    create_threads();

    std::string line;
    std::vector<std::string> fields;
    bool send_rest = false;
    bool prompt = true;
    while (1) {
      line = "";
      fields.clear();
      fields.push_back("");
      if (prompt) {
        fprintf(stderr, "> ");
        if (!str::read_line(&line, stdin, true))
          break;
        str::split(&line, " \t", true, &fields);
        if (fields.size() == 0)
          continue;
      }

      if (fields[0] == "adapt") {
        if (fields.size() != 2) {
          fprintf(stderr, "invalid adapt command\n");
          continue;
        }
        if (fields[1] == "on")
          enqueue(msg::Message(msg::M_ADAPT_ON));
        if (fields[1] == "off") 
          enqueue(msg::Message(msg::M_ADAPT_OFF));
        if (fields[1] == "reset") 
          enqueue(msg::Message(msg::M_ADAPT_RESET));
      }
      
      if (fields[0] == "pause")
        enqueue(msg::Message(msg::M_DECODER_PAUSE, true));
      if (fields[0] == "unpause")
        enqueue(msg::Message(msg::M_DECODER_UNPAUSE, true));

      if (fields[0] == "set") {
        std::string line;
        for (int i = 1; i < (int)fields.size(); i++) {
          if (i > 1)
            line.append(" ");
          line.append(fields[i]);
        }
        fprintf(stderr, "gui: sending setting \"%s\"\n", line.c_str());

        msg::Message message(msg::M_DECODER_SETTING);
        message.append(line);
        enqueue(message);
      }

      if (fields[0] == "debug") {
        std::string line;
        for (int i = 1; i < (int)fields.size(); i++) {
          if (i > 1)
            line.append(" ");
          line.append(fields[i]);
        }
        fprintf(stderr, "gui: sending debug \"%s\"\n", line.c_str());

        msg::Message message(msg::M_DEBUG);
        message.append(line);
        enqueue(message);
      }

      if (fields[0] == "s") {
        send_rest = true;
        prompt = false;
      }
      if (fields[0] == "o") {

        if (fields.size() > 1)
          snd_file = open_audio(fields[1]);

        if (snd_file != NULL) {
          if (sf_seek(snd_file, 0, SEEK_SET) < 0) {
            fprintf(stderr, "sf_seek(0) failed: %s\n", sf_strerror(snd_file));
            exit(1);
          }
        }
        continue;
      }
      if (fields[0] == "e")
	  enqueue(msg::Message(msg::M_AUDIO_END));
      if (fields[0] == "r") {
	  enqueue(msg::Message(msg::M_RESET, true));
      }
      if (send_rest || fields[0] == "a") {

        int frames = 1;
        if (fields[0] == "a" && fields.size() > 1) {
          frames = atoi(fields[1].c_str());
          if (frames <= 0)
            continue;
        }

        const int buf_size = 128 * 2; // 128 samples
        std::string buf(buf_size, ' ');
        for (int i = 0; i < frames; i++) {
          sf_count_t ret = sf_read_short(snd_file, (short int*)&buf[0], 128);
          if (sf_error(snd_file) != SF_ERR_NO_ERROR) {
            fprintf(stderr, "ERROR: sf_read_short failed: %s\n",
                    sf_strerror(snd_file));
            exit(1);
          }
          if (ret == 0) {
            fprintf(stderr, "gui: end of audio\n");
            send_rest = false;
            prompt = true;
          }
          else {
            buf.resize(ret * 2);
            msg::Message message(msg::M_AUDIO);
            message.append(buf);
            enqueue(message);
          }
        }
      }
    }

  }
  catch (std::string &str) {
    fprintf(stderr, "exception: %s\n", str.c_str());
  }
  catch (std::exception &e) {
    fprintf(stderr, "exception: %s\n", e.what());
  }
}
