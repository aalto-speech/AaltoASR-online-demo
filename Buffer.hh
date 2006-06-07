
#ifndef BUFFER_HH_
#define BUFFER_HH_

#include <stdio.h>

template <class T>
class Buffer
{

public:

  Buffer(unsigned long size);
  ~Buffer();

  // Reads from this buffer and writes it to parameter buffer.
  unsigned long read(T *buffer, unsigned long frames);

  // Writes all data from buffer to file.
  unsigned long flush(FILE *file);

  // Writes the parameter data to this->m_buffer.
  void write(const T *buffer, unsigned long frames);

  unsigned long get_size() const { return this->m_size; }
  unsigned long get_read_size() const;
  bool resize(unsigned long size);
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
