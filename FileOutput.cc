
#include "FileOutput.hh"

FileOutput::FileOutput()
//  : m_mutex(PTHREAD_MUTEX_INITIALIZER)
{
//  this->m_lock = 0;
//  this->m_mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_init(&this->m_mutex, NULL);
}

FileOutput::~FileOutput()
{
//  m_files.clear();
//  this->m_output_file = NULL;
  pthread_mutex_destroy(&this->m_mutex);
}

bool
FileOutput::write_to_files(void *data, unsigned long size, unsigned long count)
{
  std::list<FILE*>::iterator file_iterator;
/*  
  this->m_lock++;
  if (this->m_lock != 1) {
    this->m_lock--;
    fprintf(stderr, "FileOutput::write_to_files: resources were locked.\n");
    return false;
  }
//*/

  pthread_mutex_lock(&this->m_mutex);
  
  for (file_iterator = this->m_files.begin(); file_iterator != this->m_files.end(); file_iterator++) {
    fwrite(data, size, count, *file_iterator);
  }

  pthread_mutex_unlock(&this->m_mutex);
  
  //this->m_lock--;
  return true;
}

bool
FileOutput::add_file(FILE *file)
{
  /*
  this->m_lock++;
  if (this->m_lock != 1) {
    this->m_lock--;
    fprintf(stderr, "Couldn't add file in FileOutput: writing lock enabled.\n");
    return false;
  }
  //*/

  //pthread_mutex_trylock??
  pthread_mutex_lock(&this->m_mutex);
  this->m_files.push_back(file);
  pthread_mutex_unlock(&this->m_mutex);

  //this->m_lock--;
  return true;
}

bool
FileOutput::remove_file(FILE *file)
{
  /*
  this->m_lock++;
  if (this->m_lock != 1) {
    this->m_lock--;
    fprintf(stderr, "Couldn't remove file in FileOutput: writing lock enabled.\n");
    return false;
  }
  //*/

  pthread_mutex_lock(&this->m_mutex);
  this->m_files.remove(file);
  pthread_mutex_unlock(&this->m_mutex);

  //this->m_lock--;
  return true;
}

bool
FileOutput::remove_all_files()
{
/*  
  this->m_lock++;
  if (this->m_lock != 1) {
    this->m_lock--;
    fprintf(stderr, "Couldn't remove files in FileOuput: lock enabled.\n");
    return false;
  }
//*/  
  pthread_mutex_lock(&this->m_mutex);
  this->m_files.clear();
  pthread_mutex_unlock(&this->m_mutex);

  //this->m_lock--;    
  return true;
}
