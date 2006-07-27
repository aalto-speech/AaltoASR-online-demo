
#ifndef WINDOWSAVEFILE_HH_
#define WINDOWSAVEFILE_HH_

#include "WindowFile.hh"
#include "AudioInputController.hh"
#include "WindowMessageBox.hh"

class WindowSaveFile  :  public WindowFile
{

public:

  WindowSaveFile(const PG_Widget *parent, AudioInputController *audio_input);

protected:

  virtual bool do_ok();
  
private:

  bool confirm_overwrite();
  bool write_file();

  AudioInputController *m_audio_input;
  
};

#endif /*WINDOWSAVEFILE_HH_*/
