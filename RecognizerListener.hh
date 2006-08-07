
#ifndef RECOGNIZERLISTENER_HH_
#define RECOGNIZERLISTENER_HH_

#include <pthread.h>
#include "RecognitionParser.hh"
#include "msg.hh"

/** Class for reading in queue and handling the incoming messages. Operates
 * in an own thread. */
class RecognizerListener
{

public:

  /** Constructs a new in queue listener.
   * \param in_queue In queue to listen.
   * \param recognition Recognition messages are passed to this object. */
  RecognizerListener(msg::InQueue *in_queue,
                     RecognitionParser *recognition);
  /** Destructs the object. */
  ~RecognizerListener();

  /** Starts a new thread to read the in queue.
   * \return false if thread already active or thread creation failed. */
  bool start();
  /** Stops the thread. When function returns, you can be sure that the
   * thread has really finished. */
  void stop();
  /** Enables in queue reading. */
  void enable();
  /** Disables in queue reading. When disabled the thread doesn't do
   * anything. When the function returns, you can be sure that the thread has
   * really stopped reading in queue and handling the message. */
  void disable();
  
  /** If in queue has a broken pipe this flag is raised. Should be checked
   * often. This class does't do any broken pipe handling.
   * \return true if in queue has a broken pipe. */
  inline bool is_broken_pipe() const;

private:

  /** Callback function for the pthread.
   * \param user_data this pointer to the object itself.
   * \return Return value of the thread, we use NULL. */
  static void* callback(void *user_data);
  
  /** Does the reading of the in queue and handling incoming messages. If
   * in queue has a broken pipe, it throws an exception to quit the the
   * thread and raise a broken pipe flag. */
  void run() throw(msg::ExceptionBrokenPipe);

  RecognitionParser *m_recognition; //!< Object for recognition message parsing.
  msg::InQueue *m_in_queue; //!< The in queue to read.

  bool m_stop; //!< Flag telling when to quit the thread.
  bool m_enabled; //!< Flag telling if in queue should be read.
  bool m_thread_created; //!< Flag telling if thread is already active.
  bool m_broken_pipe; //!< Flag telling if in queue has a broken pipe.
  pthread_t m_thread; //!< Thread structure.
  pthread_mutex_t m_disable_lock; //!< Lock to make disabling safe.

};

bool
RecognizerListener::is_broken_pipe() const
{
  return this->m_broken_pipe;
}

#endif /*RECOGNIZERLISTENER_HH_*/
