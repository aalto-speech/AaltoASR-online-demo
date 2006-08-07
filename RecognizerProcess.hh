
#ifndef RECOGNIZERPROCESS_HH_
#define RECOGNIZERPROCESS_HH_

// Tässä vain sommittelen yhtä luokkaideaa....

class RecognizerProcess
{
  
  static const unsigned int MIN_BEAM;
  static const unsigned int MAX_BEAM;
  static const unsigned int MIN_LMSCALE;
  static const unsigned int MAX_LMSCALE;
  
  RecognizerProces(computer, script);
  ~RecognizerProcess();
  
  void start();
  void finish();

  inline msg::InQueue* get_in_queue();
  inline msg::OutQueue* get_out_queue();

  // bool for validity..
  void set_computer(const std::string &computer);
  void set_script(const std::string &script);
  bool set_beam(unsigned int beam) throw msg::ExceptionBrokenPipe;
  bool set_lmscale(unsigned int lmscale) throw msg::ExceptionBrokenPipe;

  inline const std::string& get_computer() const;
  inline const std::string& get_script() const;
  inline unsigned int get_beam() const;
  inline unsigned int get_lmscale() const;
  
private:


  Process m_process;
  msg::InQueue m_in_queue;
  msg::OutQueue m_out_queue;
//  Settings m_settings;

  std::string m_computer;
  std::string m_script;
  int m_beam;
  int m_lmscale;
};

msg::InQueue*
RecognizerProcess::get_in_queue()
{
  return &this->m_in_queue;
}
 
msg::OutQueue*
RecognizerProcess::get_out_queue()
{
  return &this->m_out_queue;
}

const std::string&
RecognizerProcess::get_computer() const
{
  return this->m_computer;
}

const std::string&
RecognizerProcess::get_script() const
{
  return this->m_script;
}

unsigned int
RecognizerProcess::get_beam() const
{
  return this->m_beam;
}

unsigned int
RecognizerProcess::get_lmscale() const
{
  return this->m_lmscale;
}

#endif /*RECOGNIZERPROCESS_HH_*/
