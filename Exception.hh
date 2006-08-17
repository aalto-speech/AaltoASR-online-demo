
#ifndef EXCEPTION_HH_
#define EXCEPTION_HH_

#include <string>

/** Base exception class derived from std::exception with a constructor that
 * allows to define the error message. */
class Exception  :  public std::exception
{
public:
  Exception(const std::string &error) throw() : m_error(error) { }
  virtual ~Exception() throw() { }
  virtual const char* what() const throw() {
    return this->m_error.data();
  }
private:
  std::string m_error;
};

#endif /*EXCEPTION_HH_*/
