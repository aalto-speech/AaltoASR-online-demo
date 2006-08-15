
#include "comparison.hh"
#include "msg.hh"
#include "str.hh"
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>

ProcessExecutor::ProcessExecutor(const char *file, const char *arg, ...)
{
  if (this->m_process.create() == 0) {
    // Child process enters here.
    
    int size;
    va_list parameter_list;
    
    // Calculate the number of parameters (+2 for arg and NULL).
    va_start(parameter_list, arg);
    for (size = 2; va_arg(parameter_list, char*) != NULL; size++);
    va_end(parameter_list);
    
    // Construct an array of the parameters.
    char **parameters = new char*[size];
    parameters[0] = new char[strlen(arg)+1];
    strcpy(parameters[0], arg);
    va_start(parameter_list, arg);
    for (int ind = 1; ind < size; ind++) {
      parameters[ind] = va_arg(parameter_list, char*);
    }
    va_end(parameter_list);
    
    // Execute the program.
    int ret_val = execvp(file, parameters);
    
    if (ret_val < 0) {
      std::string error = str::fmt(200, "Failed to execute program \"%s\"", file);
      perror(error.data());
      exit(-1);
    }
    // Child process should never get here.
    assert(false);
  }
  // Parent process continues here.
  
  // Set input non-blocking.
  msg::set_non_blocking(this->m_process.read_fd);
}

ProcessExecutor::~ProcessExecutor()
{
  this->m_process.finish();
}

bool
ProcessExecutor::read_output(std::string *output)
{
  char buffer[100];
  
  int ret_val = read(this->m_process.read_fd, buffer, 100);
  
  if (ret_val < 0) {
    if (errno == EAGAIN) // read would block
      return false;
    if (errno == EINTR) // interrupted by signal
      return false;
    perror("read_output(): read() failed");
    fprintf(stderr, "errno: %d\n", errno);
    exit(1);
  }
  
  // Process has finished, return true.
  if (ret_val == 0)
    return true;

  if (output)
    output->append(buffer, ret_val);
    
  return false;
}




TextComparisonResult
TextComparer::compare(const std::string &reference,
                      const std::string &hypothesis,
                      bool &ok) throw(Exception)
{
  TextComparer *comp = new TextComparer(reference, hypothesis);
  while (!comp->run_comparer());
  TextComparisonResult result = comp->get_result(ok);
  delete comp;
  return result;
}


bool
TextComparer::write_temp_file(std::string &filename, const std::string &content)
{
  char file[] = "tempXXXXXX";
  bool ok = false;
  int fd = mkstemp(file);
  if (write(fd, content.data(), content.size()) == (long)content.size() &&
      fsync(fd) == 0) {
        
    filename = file;
    ok = true;
  }
  close(fd);
  return ok;
}

bool
TextComparer::parse_result(TextComparisonResult::Result &result,
                           const std::string &output)
{
  std::vector<std::string> split_vector;
  std::string line;
  
  // Split the output into lines.
  str::split(&output, "\n", true, &split_vector);
  for (unsigned int ind = 0; ind < split_vector.size(); ind++) {
    // Search the line containing the scores.
    if (split_vector.at(ind).compare(0, 7, "Scores:") == 0) {
      line = split_vector.at(ind);
      str::split(&line, " ", true, &split_vector);
      // Transform each point from string to integer.
      if (split_vector.size() == 9) {
        bool ok = true;
        result.correct = str::str2long(&split_vector.at(5), &ok);
        result.substitution = str::str2long(&split_vector.at(6), &ok);
        result.deletion = str::str2long(&split_vector.at(7), &ok);
        result.insertion = str::str2long(&split_vector.at(8), &ok);
        return ok;
      }
      return false;
    }
  }
  return false;
}

void
TextComparer::clean(std::string &text, char space)
{
  // Erase non-alphabetic and non-space characters.
  for (unsigned int ind = 0; ind < text.size(); ind++) {
    char ch = text.at(ind);
    if (!isalpha(ch) && ch != 'ä' && ch != 'Ä' && ch != 'ö' && ch != 'Ö' &&
        !isspace(ch)) {

      text.erase(ind, 1);
      ind--;
    }
  }

  // Erase leading space.
  while (text.size() > 0 && isspace(text.at(0)))
    text.erase(0, 1);

  // Erase trailing space.
  while (text.size() > 0 && isspace(text.at(text.size()-1)))
    text.erase(text.size()-1, 1);

  // Convert whitespace with the given parameter space character.
  for (unsigned int ind = 0; ind < text.size(); ind++) {
    if (isspace(text.at(ind))) {
      // Convert whitespace with single space character.
      text.at(ind) = space;
      unsigned int jnd = ind + 1;
      while (jnd < text.size() && isspace(text.at(jnd)))
        text.erase(jnd, 1);
    }
  }

  // Append end of line (sclite requires this)
  text.append(1, '\n');
}

bool
TextComparer::operate(ProcessExecutor *&process,
                      std::string &output,
                      TextComparisonResult::Result &result,
                      std::string &reference_file,
                      std::string &hypothesis_file,
                      bool &parsing_ok)
{
  bool ready = true;
  if (process) {
    bool ret_val = process->read_output(&output);
    ready &= ret_val;
    if (ret_val) {
      parsing_ok = TextComparer::parse_result(result, output);
      delete process;
      process = NULL;
      remove(reference_file.data());
      remove(hypothesis_file.data());
      reference_file = "";
      hypothesis_file = "";
    }
  }
  return ready;
}

const std::string TextComparer::sclite_path = "/share/puhe/linux/bin/sclite";
  
TextComparer::TextComparer(const std::string &reference,
                           const std::string &hypothesis)
  throw(Exception)
{
  this->m_result._reference = reference;
  this->m_result._hypothesis = hypothesis;
  
  // Clean strings...
  std::string reference_word = reference;
  std::string hypothesis_word = hypothesis;
  std::string reference_char = reference;
  std::string hypothesis_char = hypothesis;
  TextComparer::clean(reference_word, ' ');
  TextComparer::clean(hypothesis_word, ' ');
  TextComparer::clean(reference_char, '_');
  TextComparer::clean(hypothesis_char, '_');

  // This is a SCLite restriction: No empty files..
  if (reference_word == "\n" || hypothesis_word == "\n" ||
      reference_char == "\n" || hypothesis_char == "\n")
  {
    throw ExceptionEmptyText();
  }
  
  // Create temporary files for strings.
  if (!TextComparer::write_temp_file(this->m_word_ref_file, reference_word) ||
      !TextComparer::write_temp_file(this->m_word_hyp_file, hypothesis_word) ||
      !TextComparer::write_temp_file(this->m_char_ref_file, reference_char) ||
      !TextComparer::write_temp_file(this->m_char_hyp_file, hypothesis_char))
  {
    remove(this->m_word_ref_file.data());
    remove(this->m_word_hyp_file.data());
    remove(this->m_char_ref_file.data());
    remove(this->m_char_hyp_file.data());
    throw ExceptionTemporaryFileFailure();
  }
  
  // Run SCLite...
  this->m_word_process = new ProcessExecutor(TextComparer::sclite_path.data(),
                                             "sclite",
                                             "-r",
                                             this->m_word_ref_file.data(),
                                             "-h",
                                             this->m_word_hyp_file.data(),
                                             "-o",
                                             "pralign",
                                             "stdout",
                                             "-i",
                                             "rm",
                                             NULL);

  this->m_char_process = new ProcessExecutor(TextComparer::sclite_path.data(),
                                             "sclite",
                                             "-r",
                                             this->m_char_ref_file.data(),
                                             "-h",
                                             this->m_char_hyp_file.data(),
                                             "-o",
                                             "pralign",
                                             "stdout",
                                             "-c",
                                             "-i",
                                             "rm",
                                             NULL);

  this->m_word_parsing_ok = false;
  this->m_char_parsing_ok = false;

}

TextComparer::~TextComparer()
{
  remove(this->m_word_ref_file.data());
  remove(this->m_word_hyp_file.data());
  remove(this->m_char_ref_file.data());
  remove(this->m_char_hyp_file.data());
  delete this->m_word_process;
  delete this->m_char_process;
}

bool
TextComparer::run_comparer()
{
  bool ready = true;

  ready &= TextComparer::operate(this->m_word_process,
                                 this->m_word_output,
                                 this->m_result._word,
                                 this->m_word_ref_file,
                                 this->m_word_hyp_file,
                                 this->m_word_parsing_ok);

  ready &= TextComparer::operate(this->m_char_process,
                                 this->m_char_output,
                                 this->m_result._char,
                                 this->m_char_ref_file,
                                 this->m_char_hyp_file,
                                 this->m_char_parsing_ok);

  return ready;
}
