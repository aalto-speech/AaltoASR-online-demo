#include <fcntl.h>
#include <errno.h>
#include "msg.hh"

namespace msg {

  void
  set_non_blocking(int fd)
  {
    int result;
  
    result = fcntl(fd, F_GETFL, 0);
    if (result >= 0)
      result = fcntl(fd, F_SETFL, result | O_NONBLOCK);
    if (result < 0) {
      perror("fcntl() failed");
      exit(1);
    }
  }

  InQueue::InQueue(int fd)
    : buffer(header_size, 0), bytes_got(0), fd(fd), eof(false), 
      suspended(false)
  {
    assert(sizeof(int) == 4);
  }

  void
  InQueue::disable() 
  { 
    assert(fd >= 0);
    buffer.resize(header_size);
    queue.clear();
    bytes_got = 0;
    fd = -1; 
    eof = false;
  }

  void 
  InQueue::enable(int fd) 
  { 
    assert(this->fd < 0);
    assert(!eof);
    //if (eof)
      //throw ExceptionBrokenPipe(fd);
    this->fd = fd; 
  }

  void
  InQueue::flush()
  {
    assert(!eof);
    //if (eof)
      //throw ExceptionBrokenPipe(fd);
      
    if (fd < 0)
      return;

    while (1) {
      assert((int)buffer.size() >= header_size);
      ssize_t ret;
      assert(bytes_got < (int)buffer.size());
      int bytes_left = buffer.size() - bytes_got;

      // Read buffer with restart
      while (1) {
      	ret = read(fd, &buffer[bytes_got], bytes_left);
        if (ret < 0) {
      	  if (errno == EAGAIN) // read would block
      	    return;
      	  if (errno == EINTR) // interrupted by signal
      	    continue;
      	  perror("flush_read(): read() failed");
          fprintf(stderr, "errno: %d\n", errno);
      	  exit(1);
      	}
      	if (ret == 0) { // end of file
      	  eof = true;
      	  return;
      	}
      	break;
      }

      assert(ret <= bytes_left);
      bytes_got += ret;
      if (ret == bytes_left) {

      	if (bytes_got == header_size) {
      	  int length = endian::get4<int>(&buffer[0]);
      	  if (length < header_size) {
      	    fprintf(stderr, "InQueue::flush() got message length %d\n",  length);
      	    exit(1);
      	  }

      	  buffer.resize(length);
      	  if (bytes_got < length)
      	    continue;
      	}
      
      	Message message;
      	message.buf = buffer;
        if (message.urgent()) {
          std::deque<Message>::iterator iter;
          // Find the position just before first non-urgent message.
          for (iter = this->queue.begin(); iter != this->queue.end(); iter++) {
            if (!iter->urgent())
              break;
          }
          this->queue.insert(iter, message);
        }
        else {
          this->queue.push_back(message);
        }

      	buffer.resize(header_size);
      	bytes_got = 0;
      	break;
      }
    }
  }

  OutQueue::OutQueue(int fd)
    : bytes_sent(0), fd(fd)
  {
    assert(sizeof(int) == 4);
  }

  void 
  OutQueue::disable() 
  { 
    assert(fd >= 0);
    queue.clear();
    buffer.clear();
    bytes_sent = 0;
    fd = -1; 
  }

  void
  OutQueue::enable(int fd) 
  { 
    assert(this->fd < 0);
    this->fd = fd; 
  }

  bool
  OutQueue::prepare_next()
  {
    if (buffer.empty()) {
      assert(bytes_sent == 0);
      if (queue.empty())
        return false;
      msg::Message &message = queue.front();
      buffer = message.buf;

      if (message.raw)
        buffer.erase(0, msg::header_size);
      else {
        int length = endian::get4<int>(&buffer[0]);
        if (length != (int)buffer.size()) {
          fprintf(stderr, "OutQueue::prepare_next() buffer size is %d bytes, "
                  "but header says %d bytes\n", (int)buffer.size(), length);
          exit(1);
        }
      }

      queue.pop_front();
      assert((int)buffer.size() > 0);
    }
    return true;
  }

  bool
  OutQueue::send_next() throw(ExceptionBrokenPipe)
  {
    assert(fd >= 0);

    assert(bytes_sent < buffer.length());
    int bytes_left = buffer.length() - bytes_sent;
    assert(bytes_left > 0);

    // Write buffer with restart
    ssize_t ret;
    while (1) {
      while (1) {
        ret = write(fd, &buffer[bytes_sent], bytes_left);
        if (ret < 0) {
          if (errno == EAGAIN) // write would block
            return false;
          if (errno == EINTR) // interrupted by signal
            continue;
          perror("flush_send(): write() failed");
          // Throw broken pipe exception.
          if (errno == EPIPE || errno == EINVAL) {
            if (errno == EINVAL) {
              fprintf(stderr, "Got EINVAL=%d, but throwing broken pipe "
                              "exception.\n", errno);
            }
            throw ExceptionBrokenPipe(fd);
          }
          exit(1);
        }
        break;
      }

      assert(ret <= bytes_left);
      bytes_sent += ret;
      if (ret == bytes_left) {
        buffer.clear();
        bytes_sent = 0;
        return true;
      }
    }
    assert(false);
  }

  void
  OutQueue::flush() throw(ExceptionBrokenPipe)
  {
    while (!queue.empty()) {
      prepare_next();
      if (!send_next())
        return;
    }
  }

  void
  OutQueue::add_message(const Message &msg)
  {
    if (msg.urgent()) {
      std::deque<Message>::iterator iter;
      // Find the position just before first non-urgent message.
      for (iter = this->queue.begin(); iter != this->queue.end(); iter++) {
        if (!iter->urgent())
          break;
      }
      this->queue.insert(iter, msg);
    }
    else {
      this->queue.push_back(msg);
    }
  }
  
  void
  OutQueue::clear_non_urgent()
  {
    std::deque<Message>::iterator iter;
    // Find the position just before first non-urgent message.
    for (iter = this->queue.begin(); iter != this->queue.end(); iter++) {
      if (!iter->urgent())
        break;
    }
    this->queue.erase(iter, this->queue.end());
  }
  

  Mux::Mux()
    : max_fd(-1)
  {
  }

  void // private
  Mux::create_fd_sets()
  {
    FD_ZERO(&read_set);
    FD_ZERO(&write_set);

    max_fd = -1;

    select_in_queues.clear();
    for (int i = 0; i < (int)in_queues.size(); i++) {
      if (in_queues[i]->is_suspended())
        continue;
      int fd = in_queues[i]->get_fd();
      if (fd < 0)
	continue;
      assert(!in_queues[i]->get_eof());
      FD_SET(fd, &read_set);
      max_fd = std::max(max_fd, fd);
      select_in_queues.push_back(in_queues[i]);
    }

    select_out_queues.clear();
    for (int i = 0; i < (int)out_queues.size(); i++) {
      if (out_queues[i]->empty())
	continue;
      int fd = out_queues[i]->get_fd();
      if (fd < 0)
	continue;
      FD_SET(fd, &write_set);
      max_fd = std::max(max_fd, fd);
      select_out_queues.push_back(out_queues[i]);
    }
  }

  void
  Mux::wait_and_flush()
  {
    bool message_pending = false;
    while (!message_pending) {
      create_fd_sets();
      assert(max_fd >= 0);

      int ret = select(max_fd + 1, &read_set, &write_set, NULL, NULL);

      if (ret < 0) {
	if (errno == EINTR)
	  continue;
	perror("Mux(): select() failed");
	exit(1);
      }
      
      if (ret == 0)
	continue;

      for (int i = 0; i < (int)select_out_queues.size(); i++) {
	OutQueue *queue = select_out_queues[i];
	if (FD_ISSET(queue->get_fd(), &write_set))
	  queue->flush();
      }

      for (int i = 0; i < (int)select_in_queues.size(); i++) {
        InQueue *queue = select_in_queues[i];
        if (FD_ISSET(queue->get_fd(), &read_set)) {
          queue->flush();
          if (!queue->empty() || queue->get_eof())
            message_pending = true;
        }
      }
    }
  }

}
