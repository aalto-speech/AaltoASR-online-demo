//*
#ifndef SETTINGS_HH_
#define SETTINGS_HH_

#include <string>
#include "msg.hh"

class Settings
{

public:

  static bool read_settings();
  static void send_settings(msg::OutQueue *out_queue) throw(msg::ExceptionBrokenPipe);
  
  static std::string ssh_to;
  static std::string script_file;
  static int beam;
  static int lmscale;
  
};

#endif /*SETTINGS_HH_*/
//*/
