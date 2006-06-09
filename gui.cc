#include "msg.hh"
#include "conf.hh"
#include "io.hh"
#include "str.hh"
#include "Process.hh"

conf::Config config;

int
main(int argc, char *argv[])
{
  try {
    config("usage: gui [OPTION...]\n")
      ('h', "help", "", "", "display help")
      ('a', "audio=FILE", "arg must", "", "audio file")
      ;

    config.default_parse(argc, argv);
    if (config.arguments.size() != 0)
      config.print_help(stderr, 1);

    io::Stream audio_stream(config["audio"].get_str());
    
    Process proc;
    if (proc.create() == 0) {
      int ret = execl("./recognizer", "./recognizer",
//                      "--verbosity", "1",
                      (char*)NULL);
      if (ret < 0) {
	perror("exec() failed");
	exit(1);
      }
      assert(false);
    }

    msg::set_non_blocking(proc.read_fd);
    msg::set_non_blocking(proc.write_fd);
    msg::InQueue rec_in_queue(proc.read_fd);
    msg::OutQueue rec_out_queue(proc.write_fd);

    int buf_size = 3200;
    std::string buf(buf_size, 0);
    std::string line;
    bool send_rest = false;
    bool prompt = true;
    while (1) {
      line = "";
      usleep(100*1000);
      if (prompt) {
        fprintf(stderr, "> ");
        if (!str::read_line(&line, stdin, true))
          break;
      }
      if (line == "s") {
        send_rest = true;
        prompt = false;
      }
      if (line == "e")
	  rec_out_queue.queue.push_back(msg::Message(msg::M_AUDIO_END));
      if (line == "r")
	  rec_out_queue.queue.push_back(msg::Message(msg::M_RESET));
      if (send_rest || line == "a") {
	size_t ret = fread(&buf[0], buf_size, 1, audio_stream);
	if (ferror(audio_stream)) {
	  perror("fread failed");
	  exit(1);
	}
	if (ret == 0) {
          fprintf(stderr, "gui: end of audio\n");
          send_rest = false;
	}
        else {
          msg::Message message(msg::M_AUDIO);
          message.append(buf);
          rec_out_queue.queue.push_back(message);
        }
      }

      rec_out_queue.flush();

      if (rec_in_queue.get_eof()) {
	fprintf(stderr, "gui: eof in rec_in_queue, exiting\n");
	exit(1);
      }
      rec_in_queue.flush();

      while (!rec_in_queue.empty()) {
	msg::Message &message = rec_in_queue.queue.front();

	std::string word(message.data_ptr(), message.data_length());
	if (message.type() == msg::M_RECOG)
	  fprintf(stderr, "gui: got RECOG: %s\n", word.c_str());

	else if (message.type() == msg::M_READY) {
	  fprintf(stderr, "gui: got READY\n");
          prompt = true;
	}

	else if (message.type() == msg::M_MESSAGE) {
          std::string str(message.data_ptr(), message.data_length());
	  fprintf(stderr, "gui: decoder says %s\n", str.c_str());
	}

	rec_in_queue.queue.pop_front();
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
