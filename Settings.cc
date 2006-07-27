
//#include <fstream>
#include <stdio.h>
#include "Settings.hh"
#include "str.hh"

std::string Settings::ssh_to = "itl_cl1";
std::string Settings::script_file = "rec.sh";
int Settings::beam = 0;
int Settings::lmscale = 0;

bool
//Settings::read_settings(const std::string &script_file)
Settings::read_settings()
{
  FILE *file;
  std::string line;
  std::vector<std::string> fields;
  bool read_settings = false;
  bool ok = true;
  
  file = fopen(script_file.data(), "r");
  if (!file) {
    fprintf(stderr, "Settings::read_settings: Error opening file.\n");
    return false;
  }

  while (!feof(file) && !read_settings) {
    str::read_line(&line, file, true);
    if (line.compare(0, 12, "./recognizer") == 0) {
      str::split(&line, " \"", true, &fields);
      for (unsigned int ind = 0; ind < fields.size() - 1; ind++) {
        if (fields[ind] == "--lm-scale") {
          lmscale = str::str2long(&fields[ind+1], &ok);
        }
        if (fields[ind] == "--beam") {
          beam = str::str2long(&fields[ind+1], &ok);
        }
      }
      read_settings = true;
    }
  }
  
  if (read_settings && ok) {
    fprintf(stderr, "Read settings. Beam: %d, LM-scale: %d\n", beam, lmscale);
  }
  else {
    read_settings = false;
    fprintf(stderr, "Settings::read_settings failed to read settings from file \"%s\"\n", script_file.data());
  }
  
  fclose(file);
  return read_settings;
}

void
Settings::send_settings(msg::OutQueue *out_queue) throw(msg::ExceptionBrokenPipe)
{
  if (out_queue) {
    msg::Message message(msg::M_DECODER_SETTING);
    char buffer[100];
    message.set_urgent(true);
    
// TODO:    if (beam on validi) ...
    sprintf(buffer, "beam %d", beam);
    message.append(buffer);
    out_queue->add_message(message);

// TODO: if lmscale on validi....
    sprintf(buffer, "lm_scale %d", lmscale);
    message.clear_data();
    message.append(buffer);
    out_queue->add_message(message);
    out_queue->flush();
  }
}
