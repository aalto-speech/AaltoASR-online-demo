#include <iostream>
#include <cstdio>
#include <pglog.h>
#include "Application.hh"
#include "conf.hh"
#include "AudioStream.hh"
#include "RecognizerStatus.hh"

using namespace std;

// cluster, beam, lmscale, int16
int main(int argc, char* argv[])
{

  try {
  conf::Config config;

  config('\0', "beam", "arg", "", "Beam value for recognizer.")
    ('\0', "lmscale", "arg", "", "Language model scale value for recognizer.")
    ('\0', "help", "", "", "Help text.")
    ('w', "width", "arg", "1024", "Window width.")
    ('h', "height", "arg", "768", "Window height.")
    ('\0', "host", "arg", "", "Recognizer is run on this computer via SSH.")
    ('\0', "script", "arg", "rec.sh", "Script file that starts recognizer.")
    ('b', "four-byte", "", "", "Allow less than 4 byte int.")
    ('d', "disable_recog", "", "", "Disables the recognizer.")
    ('s', "sample-rate", "arg", "16000", "sets the sample rate (default 16000)")
    ('\0', "words", "", "", "word based LM (without word break symbols)")
    ('\0', "connect", "arg", "", "SSH connection command, e.g. \"ssh pyramid.hut.fi ssh itl-cl1\".")
    ;

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
    cout << "This program assumes that system uses at least 4 bytes for int. "
    		"Your system uses only " << sizeof(int) << "%d.\n"
            "Program will exit.\n"
            "To ignore this error start application with -b flag.\n";
    return EXIT_FAILURE;
  }
  
  PG_LogConsole::SetLogLevel(PG_LOG_WRN);
  Application app;

  int ret_val = EXIT_SUCCESS;
  bool ok = false;
  audio::audio_sample_rate = (unsigned)config["sample-rate"].get_int();
  RecognizerStatus::words = config["words"].specified;
  
  if (config['d'].specified) {
    ok = app.initialize(argv[0],
                        config["width"].get_int(),
                        config["height"].get_int());
  }  
  else {
    // Check that parameters are specified in a correct way.
    if (!config["beam"].specified) {
      fprintf(stderr, "Beam must be specified by \"--beam\"\n");
    }
    else if (!config["lmscale"].specified) {
      fprintf(stderr, "LM-scale must be specified by \"--lmscale\"\n");
    }
    else if (config["host"].specified && config["connect"].specified) {
      fprintf(stderr, "You cannot specify both host and connect parameters.\n");
    }
    else {
      try {
        // Set the connection parameter.
        std::string connect; 
        if (config["host"].specified)
          connect = "ssh " + config["host"].get_str();
        else if (config["connect"].specified)
          connect = config["connect"].get_str();
        else // No host or connect flag specified!
          fprintf(stderr, "Warning: You are trying to run the recognizer "
                          "locally. You may want to specify either host- or "
                          "connect-flag.\n");

        ok = app.initialize(argv[0],
                            config["width"].get_int(),
                            config["height"].get_int(),
                            connect,
                            config["script"].get_str(),
                            (unsigned)config["beam"].get_int(),
                            (unsigned)config["lmscale"].get_int());
      }
      catch (Exception & e) {
        fprintf(stderr, "%s\n", e.what());
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
  catch (std::string &str) {
    fprintf(stderr, "jaakko: exception: %s\n", str.c_str());
    exit(1);
  } 
}
