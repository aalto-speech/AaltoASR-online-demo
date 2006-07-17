
#include <stdio.h>
#include "Application.hh"

int main(int argc, const char* argv[])
{
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
