
// This is .cc for template class so it should be considered as hh file.
#ifndef BUFFER_CC
#define BUFFER_CC

#include <string.h>
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
Buffer<T>::read(T *buffer, unsigned long frames)
{
  unsigned long halfing_size = 0;
  unsigned long read_size = this->get_read_size();

  // Check if there isn't anything to read.
  if (read_size == 0)
    return 0;

  // Check if trying to read more than has been written
  if (frames > read_size)
    frames = read_size;

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

  // Update read index
  this->m_read_index += frames;
  this->m_read_index %= this->m_size;

  return frames;
}

template <class T>
unsigned long
Buffer<T>::flush(FILE *file)
{
  unsigned long read_size = this->get_read_size();
  unsigned long halfing_size = 0;

  if (read_size == 0)
    return 0;

  if (this->m_write_index < this->m_read_index) {
    halfing_size = this->m_size - this->m_read_index;
  }

  if (halfing_size) {
    // write in two parts
    read_size = (fwrite(this->m_buffer + this->m_read_index, sizeof(T),
			halfing_size, file) +
		 fwrite(this->m_buffer, sizeof(T), read_size - halfing_size, file));
    
  }
  else {
    read_size = fwrite(this->m_buffer, sizeof(T), read_size, file);
  }

  this->m_read_index += read_size;
  return read_size;

}

template <class T>
void
Buffer<T>::write(const T *buffer, unsigned long frames)
{
  unsigned long halfing_size = 0;
  unsigned long old_write_index;

  if (frames == 0)
    return;

  if (frames > this->m_size)
    frames = this->m_size;

  // Check for end of buffer (need for two-part-copying)
  if (frames > this->m_size - this->m_write_index)
    halfing_size = this->m_size - this->m_write_index;
  /*
  //Could this be done without for-loop?
  old_write_index = this->m_write_index;
  for (unsigned long i = 0; i < frames; i++) {
    this->m_buffer[this->m_write_index%this->m_size] = (short)(32768.0f * buffer[i]);
    this->m_write_index = (this->m_write_index + 1) % this->m_size;
  }
  */
  ///*Old copying... Now we need to do float->short conversion
  // Write the buffer (in two parts if necessary)
  if (halfing_size && frames > halfing_size) {
    memcpy(this->m_buffer + this->m_write_index, buffer, sizeof(T) * halfing_size);
    memcpy(this->m_buffer, buffer + halfing_size, sizeof(T) * (frames - halfing_size));
  }
  else {
    memcpy(this->m_buffer + this->m_write_index, buffer, sizeof(T) * frames);
  }
  old_write_index = this->m_write_index;
  this->m_write_index += frames;
  this->m_write_index %= this->m_size;
  //*/

  // Update write index (and read index if it was passed)
  if ((old_write_index < this->m_write_index &&
       this->m_read_index > old_write_index &&
       this->m_read_index <= this->m_write_index) ||
      (old_write_index >= this->m_write_index &&
       (this->m_read_index > old_write_index ||
	this->m_read_index <= this->m_write_index))) {
    
    this->m_read_index = this->m_write_index + 1;
    this->m_read_index %= this->m_size;
  }

}

template <class T>
unsigned long
Buffer<T>::get_read_size() const
{
  if (this->m_write_index >= this->m_read_index)
    return this->m_write_index - this->m_read_index;
  else
    return this->m_size - (this->m_read_index - this->m_write_index);
}

template <class T>
bool
Buffer<T>::resize(unsigned long size)
{
  T *new_buffer = new T[size];
  unsigned long read_size = this->get_read_size();

  if (new_buffer == NULL) {
    fprintf(stderr, "Couldn't allocate memory in Buffer::resize.\n");
    return false;
  }

  // Check for dataloss
  if (size <= read_size) {
    //this->m_read_index += read_size - size + 1; // Lose the oldest data
    //this->m_read_index %= this->m_size;
    read_size = size - 1;
  }

  this->read(new_buffer, read_size);
  this->m_read_index = 0;
  this->m_write_index = read_size;

  if (this->m_buffer )
    delete this->m_buffer;
  this->m_buffer = new_buffer;
  this->m_size = size;

  return true;
}


#endif
