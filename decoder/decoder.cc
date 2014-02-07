#include <stdlib.h>
#include <Toolbox.hh>
#include "conf.hh"
#include "str.hh"
#include "msg.hh"
#include "Decoder.hh"

conf::Config config;

int
main(int argc, char *argv[])
{
  try {
    config("usage: decoder [OPTION...]\n")
      ('h', "help", "", "", "display help")
      ('v', "verbose", "", "", "print progress information")
      ('b', "binlm=FILE", "arg", "", "binary language model") 
      ('a', "lookahead=FILE", "arg", "", "binary lookahead model") 
      ('l', "lexicon=FILE", "arg", "", "lexicon") 
      ('\0', "words", "", "", "word based LM (without word break symbols)") 
      ('\0', "ph=FILE", "arg", "", "phoneme model") 
      ('\0', "dur=FILE", "arg", "", "duration model") 
      ('\0', "lm-scale=INT", "arg", "35", "language model scale (default 35)")
      ('\0', "token-limit=INT", "arg", "30000", 
       "token-limit pruning (default 30000)")
      ('\0', "beam=FLOAT", "arg", "200", 
       "beam pruning (default 200)")
      ;

    config.default_parse(argc, argv);
    if (config.arguments.size() != 0)
      config.print_help(stderr, 1);

    const char * ph;
    const char * dur = NULL;

    bool verbose = config["verbose"].specified;
    if (verbose)
      fprintf(stderr, "decoder: reading phoneme model %s\n",
    	  config["ph"].get_c_str());

    ph = config["ph"].get_c_str();


    if (config["dur"].specified) {
      if (verbose)
        fprintf(stderr, "decoder: reading duration model %s\n",
  	      config["dur"].get_c_str());
      dur = config["dur"].get_c_str();
    }

    Decoder decoder (ph, dur);
    decoder.init(config);
    decoder.run();
  }
  catch (std::string &str) {
    fprintf(stderr, "decoder: exception: %s\n", str.c_str());
    exit(1);
  }
  catch (std::exception &e) {
    fprintf(stderr, "decoder: exception: %s\n", e.what());
    exit(1);
  }
}
