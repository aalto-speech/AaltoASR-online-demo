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
      int ret = execl("./recognizer", "./recognizer", "--config", 
		      "mfcc_p_dd.feaconf", (char*)NULL);
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

    int buf_size = 1280;
    std::string buf(buf_size, 0);
    std::string line;
    while (1) {
      fprintf(stderr, "> ");
      str::read_line(&line, stdin, true);
      if (line == "r") {
	fread(&buf[0], buf_size, 1, audio_stream);
	if (ferror(audio_stream)) {
	  perror("fread failed");
	  exit(1);
	}
	msg::Message message(msg::M_AUDIO);
	message.append(buf);
	rec_out_queue.queue.push_back(message);
      }

      rec_out_queue.flush();

      if (rec_in_queue.eof) {
	fprintf(stderr, "gui: eof in rec_in_queue, exiting\n");
	exit(1);
      }
      rec_in_queue.flush();

      if (!rec_in_queue.empty()) {
	msg::Message &message = rec_in_queue.queue.front();

	fprintf(stderr, "gui: got message %d of %d bytes\n", message.type(),
	       (int)message.buf.size());

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
