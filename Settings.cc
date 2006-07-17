
//#include <fstream>
#include <stdio.h>
#include "Settings.hh"
#include "str.hh"

std::string Settings::ssh_to = "itl_cl1";
std::string Settings::script_file = "rec.sh";
long Settings::beam = 0;
long Settings::lmscale = 0;

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
/*
unsigned int
Settings::get_beam()
{
  return m_beam;
}
 
unsigned int
Settings::get_lmscale()
{
  return m_lmscale;
}

void
Settings::set_beam(unsigned int beam)
{
  m_beam = beam;
}

void
Settings::set_lmscale(unsigned int lmscale)
{
  m_lmscale = lmscale;
}
//*/
