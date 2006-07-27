
#ifndef SETTINGS_HH_
#define SETTINGS_HH_

#include <string>
#include "msg.hh"

class Settings
{

public:

//  static void set_script_file(const std::string script_file);
//  static bool read_settings(const std::string &script_file)
  static bool read_settings();
  static void send_settings(msg::OutQueue *out_queue) throw(msg::ExceptionBrokenPipe);
  
//  static bool is_valid_beam(long beam) { return beam >= 1 && beam <= 200; }
//  static bool is_valid_lmscale(long lmscale) { return lmscale >= 1 && lmscale <= 100; }

  static std::string ssh_to;
  static std::string script_file;
  static int beam;
  static int lmscale;
  
//  static unsigned int get_beam();
//  unsigned int get_lmscale();
//  void set_beam(unsigned int beam);
//  void set_lmscale(unsigned int lmscale);
};

#endif /*SETTINGS_HH_*/
