
#ifndef FILEOUTPUT_HH_
#define FILEOUTPUT_HH_

#include <pthread.h>
#include <stdio.h>
#include <list>

// Only one thread should be doing writing (this class doesn't protect
// from conflicts of multiple threads trying to write simultaneously).
// Adding and removing files are thread-safe 

class FileOutput
{
  
public:

  FileOutput();
  ~FileOutput();

  // When writing to files, you cannot add or remove files.
  bool write_to_files(void *data, unsigned long size, unsigned long count);
  
  bool add_file(FILE *file);
  bool remove_file(FILE *file);
  bool remove_all_files();
  
protected:

  std::list<FILE*> m_files;

  pthread_mutex_t m_mutex;  
//  unsigned int m_lock;
};

#endif /*FILEOUTPUT_HH_*/
