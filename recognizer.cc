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
      ('b', "hmms-base=BASENAME", "arg must", "", "basename for HMM files")
      ('d', "decoder=COMMAND", "arg must", "", "decoder command to run")
      ('C', "clusters=FILE", "arg", "", "Gaussian clustering file")
      ('\0', "eval-minc=FLOAT", "arg", "0", "minimum ratio of top clusters to evaluate")
      ('\0', "eval-ming=FLOAT", "arg", "0", "minimum ratio of Gaussians to evaluate")
      ('v', "verbosity=INT", "arg", "0", "verbosity level")
      ;

    rec.dec_command.clear();
//     rec.dec_command +=
//       "./decoder "
// //      " --verbose" 
//       " --binlm /share/work/thirsima/morph160000_4gram_1e-8.bin" 
//       " --lookahead /share/work/thirsima/morph160000_2gram.bin" 
//       " --lexicon /share/work/thirsima/morph160000.lex" 
//       " --ph " + config["hmms-base"].get_str() + ".ph"
//       " --dur " + config["hmms-base"].get_str() + ".dur"
//       " --lm-scale 35" 
//       " --token-limit 30000" 
//       " --beam 200";
      
    config.default_parse(argc, argv);
    if (config.arguments.size() != 0)
      config.print_help(stderr, 1);

    if (config["decoder"].specified)
      rec.dec_command = config["decoder"].get_str();
    rec.verbosity = config["verbosity"].get_int();

    fprintf(stderr, "rec: reading HMM model\n");
    rec.hmms.read_all(config["hmms-base"].get_str());

    if (config["clusters"].specified) {
      rec.hmms.read_clustering(config["clusters"].get_str());
      rec.hmms.set_clustering_min_evals(config["eval-minc"].get_double(),
                                        config["eval-ming"].get_double());
    }
    
    fprintf(stderr, "rec: configuring feature generator\n");
    rec.gen.load_configuration(
      io::Stream(config["hmms-base"].get_str() + ".cfg"));
    fprintf(stderr, "rec: running\n");
    rec.run();
  }
  catch (std::string &str) {
    fprintf(stderr, "rec: exception: %s\n", str.c_str());
  }
  catch (std::exception &e) {
    fprintf(stderr, "rec: exception: %s\n", e.what());
  }
}
