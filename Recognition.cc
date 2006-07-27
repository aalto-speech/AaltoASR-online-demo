
#include "Recognition.hh"

const unsigned int Recognition::frames_per_second = 125;

std::string
Recognition::get_recognition_text() const
{
  std::string text;
  unsigned int index;
//    std::vector<Morpheme>::iterator iter;
  for (index = 0; index < m_recognized.size(); index++) {
    if (m_recognized.at(index).data == "<w>") {
      text.append(" ");
    }
    else if (m_recognized.at(index).data != "<s>" && m_recognized.at(index).data != "</s>") {
      text.append(m_recognized.at(index).data);
    }
  }
  const std::vector<Morpheme> *morphemes = this->m_hypothesis.get_morphemes();
  for (index = 0; index < morphemes->size(); index++) {
    if (morphemes->at(index).data == "<w>") {
      text.append(" ");
    }
    else if (morphemes->at(index).data != "<s>" && morphemes->at(index).data != "</s>") {
      text.append(morphemes->at(index).data);
    }
  }
  return text;
}

void
Recognition::reset()
{
  this->m_recognized.clear();
  this->m_hypothesis.reset();
  this->m_recognition_frame = 0;
}

