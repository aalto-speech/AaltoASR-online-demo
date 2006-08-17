
#ifndef TEXTCOMPARISON_HH_
#define TEXTCOMPARISON_HH_

#include <string>
#include "Process.hh"
#include "Exception.hh"

// A simple process executor is declared here to make the actual comparing
// class a bit simpler. This class could be moved to a file of its own but
// because it is so small and I don't need it elsewhere, I declare it here
// to keep the number of files at least one less.

/** Simple class that executes a file in a separate process. */
class ProcessExecutor
{
public:
  // static std::string execute(const char *file, const char *arg, ...);
  
  /** Starts the execution of the given file in a separate process. The
   * parameters to the program can be given as (char*) type parameters.
   * \param file File to execute.
   * \param arg Name of the program. */
  ProcessExecutor(const char *file, const char *arg, ...);
  /** Finishes the process executing the file. */
  ~ProcessExecutor();
  /** Checks the process and writes output to given string.
   * \param output If given, output will be written into this string.
   * \return true if the process has finished execution. */
  bool read_output(std::string *output = NULL);
private:
  Process m_process; //!< Process for the execution.
};

// Few exception classes declared here.

/** Exception class for case if an error occured when writing temporary
 * files. */
class ExceptionTemporaryFileFailure  :  public Exception
{
public:
  ExceptionTemporaryFileFailure() throw()
    : Exception("Failed to write temporary files for some reason.") { }
  virtual ~ExceptionTemporaryFileFailure() throw() { }
};

/** Exception class for case if either reference or hypothesis string has no
 * actual text data. */
class ExceptionEmptyText  :  public Exception
{
public:
  ExceptionEmptyText() throw()
    : Exception("Reference and/or hypothesis must not be empty.") { }
  virtual ~ExceptionEmptyText() throw() { }
};

/** Data structure for holding the results of a text comparison. */
struct TextComparisonResult
{
  /** Data structure for one kind of comparison. */
  struct Result {
    Result() { correct = insertion = deletion = substitution = 0; }
    unsigned int correct;
    unsigned int insertion;
    unsigned int deletion;
    unsigned int substitution;
  };

  std::string _reference; //!< Reference text.
  std::string _hypothesis; //!< Hypothesis text.
  
  Result _word; //!< Results for word based comparison.
  Result _char; //!< Result for character based comparison.
  
};

/** Class for comparing two texts: reference text and hypothesis of the speech
 * recognizer. Comparing can be made in two ways: Simply call the static
 * compare function to get the result (this option blocks) or use just a bit
 * more complicated instance of the class (this option doesn't block). */
class TextComparer
{
public:
  
  /** Compares the given strings and returns the result. Blocks the current
   * process until the comparison is ready.
   * \param reference Reference text.
   * \param hypothesis Hypothesis text.
   * \param ok Set to false if comparing failed. Most probable reason is that
   *           static SCLite path is incorrect or the version is incompatible.
   *           If set to false you should not use the result information or
   *           count on it in anyway.
   * \return Result structure of the comparison.
   * \throw Throws exceptions if either string is empty or problems occured
   *        with temporary files */
  static TextComparisonResult compare(const std::string &reference,
                                      const std::string &hypothesis,
                                      bool &ok) throw(Exception);

  /** Creates a text comparer for the two given strings. Starts the necessary
   * processes. When comparing using this object, current process won't be
   * blocked. Blocking should last for a very minimal time, so consider using
   * the simpler static compare function if blocking is not critical.
   * \param reference Reference text.
   * \param hypothesis Hypothesis text.
   * \throw Throws exceptions if either string is empty or problems occured
   *        with temporary files */
  TextComparer(const std::string &reference,
               const std::string &hypothesis) throw(Exception);
  
  /** Clean up. */
  ~TextComparer();
  
  /** Because comparing is done in separate processes, we need to read its
   * output and wait for it to finish. This function does that. You won't get
   * proper results before you have called this function and it returns true.
   * \return true when comparing processes have finished. */
  bool run_comparer();
  
  /** 
   * \param ok Set to false if comparing failed. Most probable reason is that
   *           static SCLite path is incorrect or the version is incompatible.
   *           If set to false you should not use the result information or
   *           count on it in anyway.
   * \return Result structure of the comparison. */
  inline TextComparisonResult get_result(bool &ok) const;
  
private:

  /** Cleans the given text. Erases non-alphabetic characters and leading and
   * trailing whitespace. Replaces whitespace between words with single
   * character given as a parameter.
   * \param text Text to clean.
   * \param space Character to replace whitespace between words. */
  static void clean(std::string &text, char space);

  /** Writes given content into temporary file. Stores the temporary file name
   * into parameter filename.
   * \param filename Temporary file name is stored to this parameter.
   * \param content Data to write into the file.
   * \return false if failed to create the files. */
  static bool write_temp_file(std::string &filename,
                              const std::string &content);

  /** Parses SCLite's output to the given result object.
   * \param result Parsed result is stored into this.
   * \param output SCLite's output. Should have a line in format:
   *               "Scores: (#C #S #D #I) 17 4 3 1" (values are examples).
   * \return false if output wasn't in right format (maybe the execution was
   *         failed or the version is incompatible. */
  static bool parse_result(TextComparisonResult::Result &result,
                           const std::string &output);
  
  /** Operates the given process if it is hasn't been deleted: Reads the output
   * and if the process has finished, parses the result and deletes the process
   * and temporary files. Therefore parsing is done only the first time process
   * finishes.
   * \param process SCLite process to read.
   * \param output Output is stored to this.
   * \param result Result is parsed into this.
   * \param reference_file Temporary file for the reference.
   * \param hypothesis_file Temporary file for the hypothesis.
   * \param parsing_ok Set to true or false according to parsing success.
   * \return true if process has finished. */
  static bool operate(ProcessExecutor *&process,
                      std::string &output,
                      TextComparisonResult::Result &result,
                      std::string &reference_file,
                      std::string &hypothesis_file,
                      bool &parsing_ok);

  /** Path of the SCLite program. Note that the path is hard coded into this
   * source code! */
  static const std::string sclite_path;
  
  TextComparisonResult m_result; //!< Results of the comparison.
  ProcessExecutor *m_word_process; //!< Process for word comparison.
  ProcessExecutor *m_char_process; //!< Process for character comparison.
  std::string m_word_output; //!< Output of the word comparison process.
  std::string m_char_output; //!< Output of the character comparison process.
  bool m_word_parsing_ok; //!< True if word comparison output parsed done ok.
  bool m_char_parsing_ok; //!< True if char comparison output parsed done ok.
  std::string m_word_ref_file; //!< Reference temp file for word comparison.
  std::string m_word_hyp_file; //!< Hypothesis temp file for word comparison.
  std::string m_char_ref_file; //!< Reference temp file for char comparison.
  std::string m_char_hyp_file; //!< Hypothesis temp file for char comparison.
};

TextComparisonResult
TextComparer::get_result(bool &ok) const
{
  ok &= (this->m_word_parsing_ok & this->m_char_parsing_ok);
  return this->m_result;
}

#endif /*TEXTCOMPARISON_HH_*/
