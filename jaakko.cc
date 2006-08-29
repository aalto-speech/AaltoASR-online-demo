
#include <stdio.h>
#include "Application.hh"
#include "conf.hh"

// cluster, beam, lmscale, int16
int main(int argc, char* argv[])
{
  conf::Config config;

  config(0, "beam", "arg", "", "Beam value for recognizer.");
  config(0, "lmscale", "arg", "", "Language model scale value for recognizer.");
  config(0, "help", "", "", "Help text.");
  config('w', "width", "arg", "1024", "Window width.");
  config('h', "height", "arg", "768", "Window height.");
  config(0, "host", "arg", "itl-cl1", "Recognizer is run on this computer via SSH.");
  config(0, "script", "arg", "rec.sh", "Script file that starts recognizer.");
  config('b', "", "", "", "Allow less than 4 byte int.");
  config('d', "", "", "", "Disables the recognizer.");
  config(0, "route", "arg", "", "SSH connection to this computer which establishes SSH connection to cluster.");

  config.default_parse(argc, argv);
  
  // If we don't make this assumption, we would have to rewrite some ParaGUI
  // classes (at least PG_ScrollBar and PG_Rect), converting some ints to
  // Sint32. Otherwise the int range won't be enough for at least a few minutes
  // long sound clips. But yes, it is possible to use the program with only 2
  // bytes int. In that case, when (time*pixels_per_second) breaks the limits,
  // the behaviour is undefined. But it shouldn't be a big task to copy
  // the necessary ParaGUI source code files into this project, rename the
  // classes and convert ints to Sint32/Uint32.
  if (!config['b'].specified && sizeof(int) < 4) {
    fprintf(stdout, "This program assumes that system uses at least 4 bytes "
                    "for int. Your system uses only %d.\nProgram will exit.\n",
                    "To ignore this error start application with -b flag.\n",
                    sizeof(int));
    return EXIT_FAILURE;
  }
  
  Application app;
  int ret_val = EXIT_SUCCESS;
  bool ok = false;

  if (config['d'].specified) {
    ok = app.initialize(config["width"].get_int(),
                        config["height"].get_int());
  }  
  else {
    if (!config["beam"].specified) {
      fprintf(stderr, "Beam must be specified by \"--beam\"\n");
    }
    else if (!config["lmscale"].specified) {
      fprintf(stderr, "LM-scale must be specified by \"--lmscale\"\n");
    }
    else {
      try {
        std::string route = config["route"].get_str();
        ok = app.initialize(config["width"].get_int(),
                            config["height"].get_int(),
                            config["route"].specified ? &route : NULL,
                            config["host"].get_str(),
                            config["script"].get_str(),
                            (unsigned)config["beam"].get_int(),
                            (unsigned)config["lmscale"].get_int());
      }
      catch (Exception exception) {
        fprintf(stderr, "%s\n", exception.what());
      }
    }
  }
  
  if (ok) {
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
