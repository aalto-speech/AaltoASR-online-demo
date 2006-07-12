
//#include <fstream>
#include <stdio.h>
#include "Settings.hh"
#include "str.hh"

long m_beam = 0;
long m_lmscale = 0;

bool
Settings::read_settings(const std::string &script_file)
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
          m_lmscale = str::str2long(&fields[ind+1], &ok);
        }
        if (fields[ind] == "--beam") {
          m_beam = str::str2long(&fields[ind+1], &ok);
        }
      }
      read_settings = true;
    }
  }
  
  if (read_settings && ok) {
    fprintf(stderr, "Read settings. Beam: %d, LM-scale: %d\n", m_beam, m_lmscale);
  }
  else {
    read_settings = false;
    fprintf(stderr, "Settings::read_settings failed to read settings from file \"%s\"\n", script_file.data());
  }
  
  fclose(file);
  return read_settings;
}

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
