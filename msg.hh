#ifndef MESSENGER_HH
#define MESSENGER_HH

#include <string>
#include <vector>
#include <deque>
#include "endian.hh"

namespace msg {

  /* Message format:

     Bytes	Info
     4		message length in total (len)
     1		message type		
     1		urgent flag

     len - 6	data
  */

  /** Message types */
  enum { 
    M_AUDIO, // gui -> rec
    M_RESET, // gui -> rec, rec -> dec

    M_FEATURES, // rec <- ac
    M_PROBS, // rec <- ac, rec -> dec
    M_READY, // gui <- rec, rec <- dec, rec <- ac
    M_FRAME, // gui <- rec, rec <- dec
    M_RECOG, // gui <- rec, rec <- dec
  };

  const int header_size = 6;

  void set_non_blocking(int fd);

  class Message {
  public:
    std::string buf;
    bool raw;

    Message(int type = 0)
      : buf(header_size, 0), raw(false)
    {
      endian::put4(header_size, &buf[0]);
      buf.at(4) = type;
      buf.at(5) = 0;
    }

    void set_type(int type)
    {
      buf.at(4) = type;
    }

    void append(const std::string &str)
    {
      assert((int)buf.size() >= header_size);
      buf.append(str);
      endian::put4((int)buf.size(), &buf[0]);
    }

    int type()
    {
      assert((int)buf.length() >= header_size);
      return buf[4];
    }

    bool urgent() 
    { 
      assert((int)buf.length() >= header_size);
      return buf[5] != 0;
    }

    int data_length()
    {
      assert((int)buf.length() >= header_size);
      return buf.size() - header_size;
    }

    char *data_ptr()
    {
      return &buf[header_size];
    }
  };

  class InQueue {
  public:
    std::deque<Message> queue;
    std::string buffer;
    int bytes_got;
    int fd;
    bool eof;

  public:
    InQueue(int fd = -1);
    void enable(int fd);
    void disable();
    void flush();
    bool empty() { return queue.empty(); }
  };

  class OutQueue {
  public:
    std::deque<Message> queue;
    std::string buffer;
    size_t bytes_sent;
    int fd;

  public:
    OutQueue(int fd = -1);
    void flush();
    bool empty() { return queue.empty(); }
    void enable(int fd);
    void disable();
  };

  class Mux {
  public:
    std::vector<InQueue*> in_queues;
    std::vector<OutQueue*> out_queues;

  public:

    Mux();

    /** Waits and flushes queues until an input queue has messages or
     * eof pending.
     */
    void wait_and_flush();

  private:
    void create_fd_sets();

    int max_fd;
    fd_set read_set;
    fd_set write_set;
    std::vector<InQueue*> select_in_queues;
    std::vector<OutQueue*> select_out_queues;
  };

};

#endif /* MESSENGER_HH */
