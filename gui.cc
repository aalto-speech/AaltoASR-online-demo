#include <ctype.h>
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
std::string current_filename;

std::string recog_sure;
std::string recog_hypo;

SNDFILE *snd_file = NULL;

void
replace_all(std::string &str, const std::string &from, const std::string &to)
{
  std::string::size_type pos = 0;
  while (1) {
    pos = str.find(from, pos);
    if (pos == str.npos)
      return;
    str.replace(pos, from.length(), to);
    pos++;
  }
}

void
process_recog(std::string str)
{
  replace_all(str, "<w>", "_");

  std::string sure;
  std::string hypo;
  std::string::size_type pos = str.find('*');
  if (pos == str.npos)
    sure = str;
  else {
    sure = str.substr(0, pos);
    hypo = str.substr(pos + 1);
  }

  str::clean(sure, " ");
  str::clean(hypo, " ");

  std::vector<std::string> fields = str::split(sure, " \t", true);
  for (size_t i = 0; i < fields.size(); i++) {
    if (isdigit(fields[i].at(0)))
      continue;
    recog_sure.append(fields[i]);
    recog_sure.append(" ");
  }

  recog_hypo.clear();
  fields = str::split(hypo, " \t", true);
  for (size_t i = 1; i < fields.size(); i += 2) {
    if (isdigit(fields[i].at(0)))
      continue;
    recog_hypo.append(fields[i]);
    recog_hypo.append(" ");
  }
}

static void*
input_thread(void *data)
{
  fprintf(stderr, "gui: input thread created\n");
  while (!rec_in_queue.get_eof()) {
    rec_in_queue.flush();
    while (!rec_in_queue.empty()) {
      msg::Message &message = rec_in_queue.queue.front();

      std::string word(message.data_ptr(), message.data_length());
      if (message.type() == msg::M_RECOG) {
//        fprintf(stderr, "gui: got RECOG: %s\n", word.c_str());
        process_recog(word);
        fprintf(stderr, "RECOG: %s%s\n", 
                recog_sure.c_str(), recog_hypo.c_str());
      }

      else if (message.type() == msg::M_RECOG_END) {
        fprintf(stderr, "gui: got RECOG_END\n");
        recog_sure.clear();
        recog_hypo.clear();
      }
      
      else if (message.type() == msg::M_READY) {
        fprintf(stderr, "gui: got READY\n");
        recog_sure.clear();
        recog_hypo.clear();
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

void
open_audio(std::string file)
{
  if (snd_file != NULL)
    sf_close(snd_file);

  SF_INFO snd_info;
  snd_file = sf_open(file.c_str(), SFM_READ, &snd_info);
  if (snd_file == NULL) {
    fprintf(stderr, "sf_open(%s) failed\n", file.c_str());
    perror("system error:");
  }
}

int
main(int argc, char *argv[])
{
  try {
    config("usage: gui [OPTION...]\n")
      ('h', "help", "", "", "display help")
      ('a', "audio=FILE", "arg must", "", "audio file")
      ('r', "rec=FILE", "arg must", "", "rec script file")
      ('\0', "host=STR", "arg", "", "remote host")
      ('\0', "connect=STR", "arg", "", "command to connect")
      ;

    config.default_parse(argc, argv);
    if (config.arguments.size() != 0)
      config.print_help(stderr, 1);

//    audio::audio_sample_rate = (unsigned)config["sample-rate"].get_int();

    current_filename = config["audio"].get_str();
    open_audio(current_filename);
    
    Process proc;
    if (proc.create() == 0) {

      if (config["connect"].specified) {
        std::vector<std::string> fields;
        std::string connect(config["connect"].get_str());
        str::clean(connect, " \t");
        fields = str::split(connect, " \t", true);
        if (fields.empty()) {
          fprintf(stderr, "empty command in --connect\n");
          exit(1);
        }
        fields.push_back(config["rec"].get_str());
        char **argv = new char* [fields.size() + 1];
        for (size_t i = 0; i < fields.size(); i++)
          argv[i] = strdup(fields[i].c_str());
        argv[fields.size()] = NULL;
        int ret = execvp(fields[0].c_str(), argv);
        if (ret < 0) {
          perror("execvp() failed");
          exit(1);
        }
      }
      else if (config["host"].specified) {
        int ret = execlp("ssh", "ssh", config["host"].get_c_str(), 
                         config["rec"].get_c_str(), NULL);
        if (ret < 0) {
          perror("execlp() failed");
          exit(1);
        }
      }
      else {
        fprintf(stderr, "--host or --connect required\n");
        exit(1);
      }
      assert(false);
    }

    rec_in_queue.enable(proc.read_fd);

    {
      fprintf(stderr, "waiting input from recognition process\n");
      msg::Mux mux;
      mux.in_queues.push_back(&rec_in_queue);
      mux.wait_and_flush();
      fprintf(stderr, "got message\n");
    }

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
        if (!str::read_line(line, stdin, true))
          break;
        fields = str::split(line, " \t", true);

        if (fields.size() == 0) {
          send_rest = true;
          prompt = false;
          open_audio(current_filename);
        }

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

        if (fields.size() > 1) {
          open_audio(fields[1]);
          current_filename = fields[1];
        }

        open_audio(current_filename);
        continue;
      }
      if (fields[0] == "e")
	  enqueue(msg::Message(msg::M_AUDIO_END));
      if (fields[0] == "R") {
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
            msg::Message message(msg::M_AUDIO_END);
            enqueue(message);
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
