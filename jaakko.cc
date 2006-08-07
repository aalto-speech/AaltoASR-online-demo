
#include <stdio.h>
#include "Application.hh"
#include "conf.hh"

// cluster, beam, lmscale, int16
int main(int argc, char* argv[])
{
  conf::Config config;

//  conf("", "beam", "arg", "", "Beam value for recognizer.");
//  conf("", "lmscale", "arg", "", "Language model scale value for recognizer.");
  config(0, "help", "", "", "Help text.");
  config(0, "cluster", "arg", "itl-cl1", "Computer to take ssh connection and run recognizer.");
  config(0, "script", "arg", "rec.sh", "Script file that starts recognizer.");
  config('b', "", "", "", "Allow less than 4 byte int.");
  config('d', "", "", "", "Disables the recognizer.");

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
    fprintf(stdout, "This program assumes that system uses 4 bytes for int. "
                    "Your system uses only %d.\nProgram will exit.\n",
                    "To ignore this error start application with -b flag.\n",
                    sizeof(int));
    return EXIT_FAILURE;
  }
  
  Application app;
  int ret_val = EXIT_SUCCESS;
  bool ok;

  if (config['d'].specified) {
    ok = app.initialize();
  }  
  else {
    ok = app.initialize(config["cluster"].get_str(),
                        config["script"].get_str());
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
