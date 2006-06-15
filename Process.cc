#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "Process.hh"

static ssize_t
write_restart(int fd, const void *buf, size_t count)
{
  ssize_t bytes;

 again:
  bytes = write(fd, buf, count);
  if (bytes == -1 && errno == EINTR)
    goto again;

  return bytes;
}

ssize_t
read_restart(int fd, void *buf, size_t count)
{
  ssize_t retval;

 again:
  retval = read(fd, buf, count);
  if (retval == -1 && errno == EINTR)
    goto again;

  return retval;
}

static int
parent_test(int read_fd, int write_fd)
{
  ssize_t result;
  unsigned char dummy1, dummy2;

  // Any number is ok.  We just check if child returns dummy + 1
  dummy1 = 0xab; 
  result = write_restart(write_fd, &dummy1, 1);
  if (result <= 0)
    return -1;

  result = read_restart(read_fd, &dummy2, 1);
  if (result <= 0 || dummy2 != dummy1 + 1)
    return -1;
  return 0;
}

static int
child_test(int read_fd, int write_fd)
{
  ssize_t result;
  unsigned char dummy;

  // Any number is ok.  We just check if child returns dummy + 1
  result = read_restart(read_fd, &dummy, 1);
  if (result <= 0)
    return -1;
  dummy++;
  result = write_restart(write_fd, &dummy, 1);
  if (result <= 0)
    return -1;

  return 0;
}

Process::Process()
  : pid(0), read_fd(-1), write_fd(-1)
{
}

Process::~Process()
{
//  finish();
}

pid_t
Process::create()
{
  int child_pipe[2];
  int parent_pipe[2];

  // Create pipes
  if (pipe(child_pipe) < 0) {
    child_pipe[0] = -1;
    child_pipe[1] = -1;
    goto error;
  }
  if (pipe(parent_pipe) < 0) {
    parent_pipe[0] = -1;
    parent_pipe[1] = -1;
    goto error;
  }

  // Create child process
  pid = fork();
  if (pid < 0) goto error;
  if (pid == 0) {
    // Child
    close(child_pipe[1]);
    close(parent_pipe[0]);
    // FIXME!! Is this correct way to do this?
    // We want to connect child's stdin and stdout to these pipes.
    if (dup2(child_pipe[0], 0) < 0) 
      exit(-1);
    if (dup2(parent_pipe[1], 1) < 0)
      exit(-1);

    if (child_test(child_pipe[0], parent_pipe[1]) < 0)
      exit(-1);
    return 0; // CHILD RETURNS
  }
  else {
    // Parent
    if (parent_test(parent_pipe[0], child_pipe[1]) < 0) {
      kill(pid, SIGKILL);
      waitpid(pid, NULL, 0);
      goto error;
    }
    close(child_pipe[0]);
    close(parent_pipe[1]);
    read_fd = parent_pipe[0];
    write_fd = child_pipe[1];

    return pid; // PARENT RETURNS
  }

  // Error handling
 error:
  if (child_pipe[0] != -1) ::close(child_pipe[0]);
  if (child_pipe[1] != -1) ::close(child_pipe[1]);
  if (parent_pipe[0] != -1) ::close(parent_pipe[0]);
  if (parent_pipe[1] != -1) ::close(parent_pipe[1]);
  return -1;  
}

void
Process::finish()
{
  if (close(write_fd) < 0) {
    perror("close(write_fd) failed");
    exit(1);
  }

  kill(pid, SIGTERM);
  wait(&finish_result);

  if (close(read_fd) < 0) {
    perror("close(read_fd) failed");
    exit(1);
  }
}
