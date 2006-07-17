
#ifndef SETTINGS_HH_
#define SETTINGS_HH_

#include <string>

class Settings
{

public:

//  static void set_script_file(const std::string script_file);
//  static bool read_settings(const std::string &script_file)
  static bool read_settings();

  static std::string ssh_to;
  static std::string script_file;
  static long beam;
  static long lmscale;
  
//  static unsigned int get_beam();
//  unsigned int get_lmscale();
//  void set_beam(unsigned int beam);
//  void set_lmscale(unsigned int lmscale);
};

#endif /*SETTINGS_HH_*/
