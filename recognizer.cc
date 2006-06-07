#include "conf.hh"
#include "Recognizer.hh"
#include "io.hh"

Recognizer rec;
conf::Config config;

int
main(int argc, char *argv[])
{
  try {
    config("usage: acoustics [OPTION...]\n")
      ('h', "help", "", "", "display help")
      ('c', "config=FILE", "arg", "", "feature generator config")
      ;

    config.default_parse(argc, argv);
    if (config.arguments.size() != 0)
      config.print_help(stderr, 1);

    rec.gen.load_configuration(io::Stream(config["config"].get_str()));
    rec.run();
  }
  catch (std::string &str) {
    fprintf(stderr, "exception: %s\n", str.c_str());
  }
  catch (std::exception &e) {
    fprintf(stderr, "exception: %s\n", e.what());
  }
}
