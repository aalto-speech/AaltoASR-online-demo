
#ifndef BUFFER_HH_
#define BUFFER_HH_

#include <stdio.h>
#include <string>

template <class T>
class Buffer
{

public:

  Buffer(unsigned long size);
  ~Buffer();

  // Reads from this buffer and writes it to parameter buffer.
  unsigned long read(T *buffer, unsigned long frames) const;
  unsigned long read(std::string &buffer, unsigned long frames) const;
  // Writes the parameter data to this->m_buffer.
  unsigned long write(const T *buffer, unsigned long frames);
  unsigned long write(const char *buffer, unsigned long frames);

  // Writes all data from buffer to file.
//  unsigned long flush(FILE *file);

  inline unsigned long get_size() const { return this->m_size; }
  unsigned long get_read_size() const;
  unsigned long get_write_size() const;
  
//  inline unsigned long get_read_pos() const { return this->m_read_index; }
//  inline unsigned long get_write_pos() const { return this->m_write_index; }
  void move_read_pos(unsigned long add);
  void move_write_pos(unsigned long add);
  
//  bool resize(unsigned long size);
  //void move_start(); // moves reader to the beginning
  //void clear();

protected:
  
  T *m_buffer;
  unsigned long m_size;
  unsigned long m_read_index; // index to be read next
  unsigned long m_write_index; // index to be written next
};

#include "Buffer.cc"

#endif
