
#ifndef SETTINGS_HH_
#define SETTINGS_HH_

#include <string>

namespace Settings
{
  bool read_settings(const std::string &script_file);
  unsigned int get_beam();
  unsigned int get_lmscale();
  void set_beam(unsigned int beam);
  void set_lmscale(unsigned int lmscale);
};

#endif /*SETTINGS_HH_*/
