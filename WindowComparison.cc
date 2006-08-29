
#include "WindowComparison.hh"
#include "str.hh"

WindowComparison::WindowComparison(PG_Widget *parent,
                                   const std::string &reference,
                                   const std::string &hypothesis)
  : WindowChild(parent, "Comparison results", 300, 530, true, true, "OK"),
    m_reference(reference),
    m_hypothesis(hypothesis)
{
  this->m_comparer = NULL;
  this->m_waiting_label = NULL;
}

WindowComparison::~WindowComparison()
{
  if (this->m_comparer) {
    delete this->m_comparer;
    this->m_comparer = NULL;
  }
}

void
WindowComparison::initialize()
{
  WindowChild::initialize();
}

void
WindowComparison::do_opening()
{
  WindowChild::do_opening();

  this->m_waiting_label = new PG_Label(this->m_window,
                                       PG_Rect(10, 30, 100, 30),
                                       "Comparing...");
                                       
  try {
    this->m_comparer = new TextComparer(this->m_reference, this->m_hypothesis);
  }
  catch (Exception excep) {
    this->error(excep.what(), ERROR_CLOSE);
  }

}

void
WindowComparison::do_running()
{
  if (this->m_comparer) {
    if (this->m_comparer->run_comparer()) {
      bool ok = true;
      TextComparisonResult result = this->m_comparer->get_result(ok);
      if (!ok) {
        this->error("Comparing failed.\nMost probable reason is that SCLite "
                    "was not found", ERROR_CLOSE);
      }
      else {
        if (this->m_waiting_label) {
          delete this->m_waiting_label;
          this->m_waiting_label = NULL;
        }
        this->construct_result_array(result);
        this->m_window->Update(true);
      }
      delete this->m_comparer;
      this->m_comparer = NULL;
    }
  }
}

void
WindowComparison::do_closing(int return_value)
{
  if (this->m_waiting_label) {
    delete this->m_waiting_label;
    this->m_waiting_label = NULL;
  }
  if (this->m_comparer) {
    delete this->m_comparer;
    this->m_comparer = NULL;
  }
}

void
WindowComparison::construct_result_array(const TextComparisonResult &result)
{
  this->construct_label(0, 0, "WORD", PG_Label::RIGHT);
  this->construct_label(1, 0, "N", PG_Label::CENTER);
  this->construct_label(2, 0, "%", PG_Label::CENTER);
  this->construct_label(0, 1, "Accuracy", PG_Label::RIGHT);
  this->construct_label(0, 2, "Correct", PG_Label::RIGHT);
  this->construct_label(0, 3, "Substitution", PG_Label::RIGHT);
  this->construct_label(0, 4, "Deletion", PG_Label::RIGHT);
  this->construct_label(0, 5, "Insertion", PG_Label::RIGHT);

  this->construct_label(0, 7, "CHARACTER", PG_Label::RIGHT);
  this->construct_label(1, 7, "N", PG_Label::CENTER);
  this->construct_label(2, 7, "%", PG_Label::CENTER);
  this->construct_label(0, 8, "Accuracy", PG_Label::RIGHT);
  this->construct_label(0, 9, "Correct", PG_Label::RIGHT);
  this->construct_label(0, 10, "Substitution", PG_Label::RIGHT);
  this->construct_label(0, 11, "Deletion", PG_Label::RIGHT);
  this->construct_label(0, 12, "Insertion", PG_Label::RIGHT);

  this->construct_numeric_label(1, 2, result._word.correct);
  this->construct_numeric_label(1, 3, result._word.substitution);
  this->construct_numeric_label(1, 4, result._word.deletion);
  this->construct_numeric_label(1, 5, result._word.insertion);

  this->construct_numeric_label(1, 9, result._char.correct);
  this->construct_numeric_label(1, 10, result._char.substitution);
  this->construct_numeric_label(1, 11, result._char.deletion);
  this->construct_numeric_label(1, 12, result._char.insertion);

  long correct = (long)result._word.correct - result._word.insertion;
  unsigned int ref_words = result._word.correct + result._word.substitution +
                           result._word.deletion;// - result._word.insertion;
  unsigned int total = result._word.correct + result._word.substitution +
                       result._word.deletion + result._word.insertion;
  this->construct_percentage_label(2, 1, correct, ref_words);
  this->construct_percentage_label(2, 2, result._word.correct, total);
  this->construct_percentage_label(2, 3, result._word.substitution, total);
  this->construct_percentage_label(2, 4, result._word.deletion, total);
  this->construct_percentage_label(2, 5, result._word.insertion, total);
  
  correct = (long)result._char.correct - result._char.insertion;
  ref_words = result._char.correct + result._char.substitution +
              result._char.deletion;// - result._char.insertion;
  total = result._char.correct + result._char.substitution +
          result._char.deletion + result._char.insertion;
  this->construct_percentage_label(2, 8, correct, ref_words);
  this->construct_percentage_label(2, 9, result._char.correct, total);
  this->construct_percentage_label(2, 10, result._char.substitution, total);
  this->construct_percentage_label(2, 11, result._char.deletion, total);
  this->construct_percentage_label(2, 12, result._char.insertion, total);

}

PG_Label*
WindowComparison::construct_numeric_label(unsigned int column,
                                          unsigned int row,
                                          unsigned int value)
{
  return this->construct_label(column,
                               row,
                               str::fmt(10, "%d", value),
                               PG_Label::CENTER);
}

PG_Label*
WindowComparison::construct_percentage_label(unsigned int column,
                                             unsigned int row,
                                             long value,
                                             unsigned int total)
{
  float percent = 0;

  if (total > 0)
    percent = ((float)value / total) * 100;
    
  return this->construct_label(column,
                               row,
                               str::fmt(10, "%.1f%%", percent),
                               PG_Label::CENTER);
}


PG_Label*
WindowComparison::construct_label(unsigned int column,
                                  unsigned int row,
                                  const std::string &text,
                                  PG_Label::TextAlign align)
{
  const unsigned int left_margin = 30;
  const unsigned int top_margin = 35;
  const unsigned int horizontal_space = 10;
  const unsigned int vertical_space = 5;
  const unsigned int first_extra_width = 40;
  const unsigned int first_extra_space = 10;
  const unsigned int width = 60;
  const unsigned int height = 25;
  unsigned int x = left_margin +
                   (column > 0) * (first_extra_width + first_extra_space) +
                   column * (width + horizontal_space);
  unsigned int y = top_margin + row * (height + vertical_space);
  
  PG_Label *label = new PG_Label(this->m_window,
                                 PG_Rect(x,
                                         y,
                                         width + (column == 0) * first_extra_width,
                                         height),
                                 text.data());
  label->SetAlignment(align);
  label->SetVisible(true);
  return label;
}
