#ifndef PROCESS_HH
#define PROCESS_HH

#include <sys/types.h>
#include <unistd.h>

class Process {
public:
  Process();
  ~Process();
  
  /** Forks a new process.  The standard input and output of the
   * process is connected to \ref read_fd and \ref write_fd. 
   * \return 0 for child and pid of the child for parent
   */
  pid_t create();

  /** Finish the process. */
  void finish();

  pid_t pid; //!< pid of the child
  int read_fd; //!< File descriptor for getting output of the child
  int write_fd; //!< File descriptor for writing input for the child
  int finish_result; //!< Result of wait() after calling finish() 
};

#endif /* PROCESS_HH */
