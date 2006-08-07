
#ifndef BUFFER_HH_
#define BUFFER_HH_

#include <stdio.h>
#include <string>

/** Ring buffer. You can read from the buffer or write to the buffer. After
 * data has been written it can be read, and after data has been read it can
 * be overwritten. Same data can only be read once.
 * 
 * Read and write functions are thread-safe as long as there is only one thread
 * writing and one thread reading at the same time (exception: when clear
 * function is used no other thread should be reading at the same time). */
template <class T>
class Buffer
{

public:

  /** Constructs new ring buffer.
   * \param size Size of the buffer. */
  Buffer(unsigned long size);
  /** Destructs the buffer. */
  ~Buffer();

  /** Reads data from this buffer and writes it to the parameter buffer.
   * \param buffer Writes data to this buffer.
   * \param frames Maximum number of frames to write. 
   * \return Number of frames copied. */
  unsigned long read(T *buffer, unsigned long frames);
  /** Reads data from this buffer and writes it to the parameter buffer.
   * \param buffer Writes data to this buffer.
   * \return Number of frames copied. */
  unsigned long read(std::string &buffer);
  /** Reads data from parameter buffer and writes it to the this buffer.
   * \param buffer Reads data from this buffer.
   * \param frames Maximum number of frames to copy.
   * \return Number of frames copied. */
  unsigned long write(const T *buffer, unsigned long frames);

  /** \return Size of the buffer. */
  inline unsigned long get_size() const;
  /** \return Number of frames read from this buffer. clear() resets. */
  inline unsigned long get_frames_read() const;
  
  /** Moves read cursor to same location with write cursor, thus making
   * read size zero. Make sure no other threads are reading while one is
   * clearing! */  
  void clear();

private:
  
  /** \return Number of frames that can be read. */
  inline unsigned long get_read_size() const;
  /** \return Number of frames that can be written. */
  inline unsigned long get_write_size() const;
  /** \param add Number of frames to move the read cursor. */
  void move_read_pos(unsigned long add);
  /** \param add Number of frames to move the write cursor. */
  void move_write_pos(unsigned long add);

  /** Reads data from this buffer and writes it to the parameter buffer.
   * This does only the reading, no cursor movements or size checks.
   * \param buffer Writes data to this buffer.
   * \param frames Maximum number of frames to write. 
   * \return Number of frames copied. */
  unsigned long do_reading(T *buffer, unsigned long frames) const;
  /** Reads data from this buffer and writes it to the parameter buffer.
   * This does only the reading, no cursor movements or size checks.
   * \param buffer Writes data to this buffer.
   * \return Number of frames copied. */
  unsigned long do_reading(std::string &buffer, unsigned long frames) const;
  /** Reads data from parameter buffer and writes it to the this buffer.
   * This does only the writing, no cursor movements or size checks.
   * \param buffer Reads data from this buffer.
   * \param frames Maximum number of frames to copy.
   * \return Number of frames copied. */
  unsigned long do_writing(const T *buffer, unsigned long frames);

  T *m_buffer; //!< Array for the buffer.
  unsigned long m_size; //!< Size of the array.
  unsigned long m_read_index; //!< Index to be read next.
  unsigned long m_write_index; //!< Index to be written next.
  unsigned long m_frames_read; //!< Counter of frames read. (clear() resets)
};

// Because this is a template class, all the implementations should be in
// the header file.
#include "Buffer.cc"

#endif
