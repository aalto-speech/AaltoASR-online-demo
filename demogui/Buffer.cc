
// This is .cc for template class so it should be considered as hh file.
#ifndef BUFFER_CC
#define BUFFER_CC

#include <string.h>
#include <assert.h>
#include "Buffer.hh"

template <class T>
Buffer<T>::Buffer(unsigned long size)
{
  this->m_buffer = new T[size];
  if (!this->m_buffer) {
    fprintf(stderr, "Fatal error in Buffer constructor:\nOut of memory.\n");
  }
  this->m_size = size;
  this->m_read_index = 0;
  this->m_write_index = 0;
  this->m_frames_read = 0;
}

template <class T>
Buffer<T>::~Buffer()
{
  if (this->m_buffer) {
    delete [] this->m_buffer;
    this->m_buffer = NULL;
  }
}

template <class T>
unsigned long
Buffer<T>::read(T *to, unsigned long frames)
{
  unsigned long read_size = this->get_read_size();
  if (read_size > frames)
    read_size = frames;
  
  // We can read without locking, because writing is done to different part
  read_size = this->do_reading(to, read_size);
  
  // Move read cursor
  this->move_read_pos(read_size);
  
  return read_size;
}

template <class T>
unsigned long
Buffer<T>::read(std::string &to)
{
  unsigned long read_size = this->get_read_size();

  // We can read without locking, because writing is done to different part
  read_size = this->do_reading(to, read_size);
  
  // Move read cursor
  this->move_read_pos(read_size);
  
  return read_size;
}

template <class T>
unsigned long
Buffer<T>::write(const T *from, unsigned long frames)
{
  unsigned long write_size = this->get_write_size();
  if (write_size > frames)
    write_size = frames;
  
  // We can write without locking, because reading is done to different part
  write_size = this->do_writing(from, write_size);
  
  // Move read cursor
  this->move_write_pos(write_size);
  
  return write_size;
}

template <class T>
unsigned long
Buffer<T>::do_reading(T *buffer, unsigned long frames) const
{
  unsigned long halfing_size = 0;
  
  if (frames >= this->m_size) {
    frames = this->m_size - 1;
    assert(false); // Use assert just to notice if this occurs.
  }

  // Check for end of buffer (need for two-part-copying)
  if (frames > this->m_size - this->m_read_index)
    halfing_size = this->m_size - this->m_read_index;

  // Read the data (in two parts if necessary)
  if (halfing_size && frames > halfing_size) {
    memcpy(buffer, this->m_buffer + this->m_read_index, sizeof(T) * halfing_size);
    memcpy(buffer + halfing_size, this->m_buffer, sizeof(T) * (frames - halfing_size));
  }
  else {
    memcpy(buffer, this->m_buffer + this->m_read_index, sizeof(T) * frames);
  }
  return frames;
}

template <class T>
unsigned long
Buffer<T>::do_reading(std::string &buffer, unsigned long frames) const
{
  unsigned long halfing_size = 0;

  if (frames >= this->m_size) {
    frames = this->m_size - 1;
    assert(false); // Use assert just to notice if this occurs.
  }

  // Check for end of buffer (need for two-part-copying)
  if (frames > this->m_size - this->m_read_index)
    halfing_size = this->m_size - this->m_read_index;

  // Read the data (in two parts if necessary)
  if (halfing_size && frames > halfing_size) {
    buffer.append((char*)(this->m_buffer + this->m_read_index), sizeof(T) * halfing_size);
    buffer.append((char*)this->m_buffer, sizeof(T) * (frames - halfing_size));
  }
  else {
    buffer.append((char*)(this->m_buffer + this->m_read_index), sizeof(T) * frames);
  }
  return frames;
}

template <class T>
unsigned long
Buffer<T>::do_writing(const T *buffer, unsigned long frames)
{
  unsigned long halfing_size = 0;

  if (frames >= this->m_size) {
    frames = this->m_size - 1;
    fprintf(stderr, "Warning: Trying to write to buffer more than its size.\n");
    assert(false); // Use assert just to notice if this occurs.
  }
  
  // Check for end of buffer (need for two-part-copying)
  if (frames > this->m_size - this->m_write_index)
    halfing_size = this->m_size - this->m_write_index;
    
  // Write the buffer (in two parts if necessary)
  if (halfing_size) {
    memcpy(this->m_buffer + this->m_write_index, buffer, sizeof(T) * halfing_size);
    memcpy(this->m_buffer, buffer + halfing_size, sizeof(T) * (frames - halfing_size));
  }
  else {
    memcpy(this->m_buffer + this->m_write_index, buffer, sizeof(T) * frames);
  }
  return frames;
}

template <class T>
void
Buffer<T>::clear()
{
  this->m_read_index = this->m_write_index;
  this->m_frames_read = 0;
}

template <class T>
unsigned long
Buffer<T>::get_size() const
{
  return this->m_size;
}

template <class T>
unsigned long
Buffer<T>::get_frames_read() const
{
  return this->m_frames_read;
}

template <class T>
unsigned long
Buffer<T>::get_read_size() const
{
  return (this->m_size + this->m_write_index - this->m_read_index) % this->m_size;
}

template <class T>
unsigned long
Buffer<T>::get_write_size() const
{
  return (this->m_size + this->m_read_index - this->m_write_index - 1) % this->m_size;
}

template <class T>
void
Buffer<T>::move_read_pos(unsigned long add)
{
  this->m_read_index = (this->m_read_index + add) % this->m_size;
  this->m_frames_read += add;
}

template <class T>
void
Buffer<T>::move_write_pos(unsigned long add)
{
  this->m_write_index = (this->m_write_index + add) % this->m_size;
}

#endif
