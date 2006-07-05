
#ifndef RECOGNITION_HH_
#define RECOGNITION_HH_

#include <vector>
#include <string>

struct Morpheme
{
  unsigned long time;
  unsigned long duration;
  std::string data;
};

class Recognition
{
  
public:

  static const unsigned int frames_per_second;

  void add_morpheme(const Morpheme &morpheme)
  {
    this->m_morphemes.push_back(morpheme);
  }
  
  void add_morpheme(const std::string &data, unsigned long time, unsigned long duration)
  {
    Morpheme morph;
    morph.data = data;
    morph.time = time;
    morph.duration = duration;
    this->m_morphemes.push_back(morph);
  }
  
  void reset()
  {
    this->m_morphemes.clear();
  }
  
  inline const std::vector<Morpheme>* get_morphemes() const
  {
    return &this->m_morphemes;
  }
  
private:

  std::vector<Morpheme> m_morphemes;
//  unsigned long
  
};

#endif /*RECOGNITION_HH_*/
