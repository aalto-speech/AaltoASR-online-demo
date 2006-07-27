
#include <stdio.h>
#include "Application.hh"

int main(int argc, const char* argv[])
{
  // If we don't make this assumption, we would have to rewrite some ParaGUI
  // classes (PG_ScrollBar, PG_ScrollArea, PG_Rect), converting some ints to
  // Sint32. Otherwise the int range won't be enough for at least a few minutes
  // long sound clips. But yes, it is possible to use the program with only 2
  // bytes int. In that case, when (time*pixels_per_second) breaks the limits,
  // the behaviour is undefined. But it shouldn't be a big task to copy
  // the necessary ParaGUI source code files into this project, rename the
  // classes and convert ints to Sint32/Uint32.
  if (sizeof(int) < 4) {
    fprintf(stdout, "This program assumes that system uses 4 bytes for int. "
                    "Your system uses only %d.\nProgram will exit.\n",
                    "For more information, see the source code file jaakko.cc\n",
                    sizeof(int));
    return EXIT_FAILURE;
  }
  
  Application app;
  int ret_val = EXIT_SUCCESS;
  std::string ssh_to = "itl-cl1";
  std::string script_file = "/home/jluttine/workspace/online-demo/rec.sh";

  if (argc >= 2) ssh_to = argv[1];
  if (argc >= 3) script_file = argv[2];

  if (app.initialize(ssh_to, script_file)) {
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
