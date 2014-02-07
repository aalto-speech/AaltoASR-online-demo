
#ifndef WINDOWTEXTCOMPARISON_HH_
#define WINDOWTEXTCOMPARISON_HH_

#include <pglabel.h>
#include "WindowChild.hh"
#include "comparison.hh"

/** A window for executing the comparison and showing the comparison results. */
class WindowComparison  :  public WindowChild
{
  
public:

  /** 
   * \param parent Parent widget.
   * \param reference Reference text in comparison.
   * \param hypothesis Hypothesis text in comparison. */
  WindowComparison(PG_Widget *parent,
                   const std::string &reference,
                   const std::string &hypothesis);
  virtual ~WindowComparison();
  
  virtual void initialize();
  
protected:

  /** Starts comparison process. */
  virtual void do_opening();
  /** Waits for comparison, and prints the results when ready. */
  virtual void do_running();
  virtual void do_closing(int return_value);
  
  void construct_result_array(const TextComparisonResult &result);
  
  PG_Label* construct_numeric_label(unsigned int column,
                                    unsigned int row,
                                    unsigned int value);
  
  PG_Label* construct_percentage_label(unsigned int column,
                                       unsigned int row,
                                       long value,
                                       unsigned int total);

  PG_Label* construct_label(unsigned int column,
                            unsigned int row,
                            const std::string &text,
                            PG_Label::TextAlign align);

private:
  
  const std::string m_reference;
  const std::string m_hypothesis;

  PG_Label *m_waiting_label; //!< Text label when waiting the comparison.
  TextComparer *m_comparer; //!< Reference-hypothesis comparer.
};

#endif /*WINDOWTEXTCOMPARISON_HH_*/
