
#ifndef RECOGNIZERPROCESS_HH_
#define RECOGNIZERPROCESS_HH_

#include "Process.hh"
#include "msg.hh"
#include "Exception.hh"

/** Class for the recognizer process. Starts and finishes the process and
 * stores the communication queues. Also stores some parameters for the
 * recognizer. */
class RecognizerProcess
{
  
public:

  // Valid values for parameters.
  static const unsigned int MIN_BEAM; //!< Minimum beam value.
  static const unsigned int MAX_BEAM; //!< Maximum beam value.
  static const unsigned int MIN_LMSCALE; //!< Minimum LM-scale value.
  static const unsigned int MAX_LMSCALE; //!< Maximum LM-scale value.
  
  /** Constructs the object.
   * \param route Optional computer to which the first SSH connection is made,
   *              and then connects to cluster computer.
   * \param cluster Recognizer is run on this computer via SSH.
   * \param script This file will be run on the computer.
   * \param beam Beam value.
   * \param lmscale LM-scale value. */
  RecognizerProcess(const std::string &connect,
                    const std::string &script,
                    unsigned int beam,
                    unsigned int lmscale) throw(Exception);
  /** Destructs the object. */
  ~RecognizerProcess() { }
  
  /** Starts a new process for the recognizer and constructs the communication
   * queues.
   * Note 1: Same queue pointers are valid!
   * Note 2: Function doesn't synchronize member variable parameters with the
   *         recognizer. You should use either read_settings or send_settings
   *         to do that. */
  void start();
  /** Finishes the recognizer process. */
  void finish();

  /** \return Input queue from the recognizer. */
  inline msg::InQueue* get_in_queue();
  /** \return Output queue to the recognizer. */
  inline msg::OutQueue* get_out_queue();

  /** Sends member variable parameters to out queue. Doesn't flush the queue,
   * so messages aren't really sent. You should do the flushing.
   * Note: Doesn't flush because making sure the messages are really flushed
   *       could block the thread. */
  void send_settings();
  /** Reads parameter values from the script file that launched the recognizer.
   * \return false if failed to read the parameter values. */
//  bool read_settings();

  /** Tries to set the computer which the ssh connection is established to.
   * \param computer SSH connection will be established to this computer.
   * \return false if process is started. */
//  bool set_computer(const std::string &computer);
  /** Tries to set the script file which is run on the ssh computer.
   * \param script The file to launch the recognizer.
   * \return false if process is started. */
//  bool set_script(const std::string &script);
  /** Tries to set the beam parameter. Puts the message into out queue but
   * doesn't flush.
   * \param beam Beam value.
   * \return false if parameter value was invalid. */
  bool set_beam(unsigned int beam);
  /** Tries to set the LM-scale parameter. Puts the message into out queue but
   * doesn't flush.
   * \param lmscale LM-scale value.
   * \return false if parameter value was invalid. */
  bool set_lmscale(unsigned int lmscale);

  /** \return SSH connection is established to this computer. */
//  inline const std::string& get_computer() const;
  /** \return This script file is run to launch the recognizer. */
//  inline const std::string& get_script() const;
  /** \return Beam value. */
  inline unsigned int get_beam() const;
  /** \return LM-scale value. */
  inline unsigned int get_lmscale() const;
  
private:

  /** Sends a parameter change message to out queue.
   * \param name Name of the parameter.
   * \param value Value of the parameter. */
  void send_parameter(const std::string &name, unsigned int value);

  Process m_process; //!< The process for recognizer.
  msg::InQueue m_in_queue; //!< In queue from recognizer.
  msg::OutQueue m_out_queue; //!< Out queue to recognizer.
  
  std::string m_connect; //!< Optional SSH connection command.
  std::string m_script; //!< Script file used to launch the recognizer.
  int m_beam; //!< Beam parameter value.
  int m_lmscale; //!< LM-scale parameter value.
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
/*
const std::string&
RecognizerProcess::get_computer() const
{
  return this->m_cluster;
}

const std::string&
RecognizerProcess::get_script() const
{
  return this->m_script;
}
//*/
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
