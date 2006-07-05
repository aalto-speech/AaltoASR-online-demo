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

int
main(int argc, char *argv[])
{
  try {
    config("usage: gui [OPTION...]\n")
      ('h', "help", "", "", "display help")
      ('a', "audio=FILE", "arg must", "", "audio file")
      ('\0', "host=STR", "arg must", "", "remote host")
      ;

    config.default_parse(argc, argv);
    if (config.arguments.size() != 0)
      config.print_help(stderr, 1);

    io::Stream audio_stream(config["audio"].get_str());
    
    Process proc;
    if (proc.create() == 0) {
      int ret = execlp("ssh", "ssh", config["host"].get_c_str(), 
                       "/home/jluttine/workspace/online-demo/rec.sh", NULL);
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

      if (fields[0] == "s") {
        send_rest = true;
        prompt = false;
      }
      if (fields[0] == "o") {
        audio_stream.close();
        audio_stream.open(config["audio"].get_str());
        continue;
      }
      if (fields[0] == "e")
	  enqueue(msg::Message(msg::M_AUDIO_END));
      if (fields[0] == "r")
	  enqueue(msg::Message(msg::M_RESET));
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
          size_t ret = fread(&buf[0], buf_size, 1, audio_stream);
          if (ferror(audio_stream)) {
            perror("fread failed");
            exit(1);
          }
          if (ret == 0) {
            fprintf(stderr, "gui: end of audio\n");
            send_rest = false;
            prompt = true;
          }
          else {
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
