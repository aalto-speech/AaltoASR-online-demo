
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

class Hypothesis
{
public:
  inline void add_morpheme(const Morpheme &morpheme) {
    this->m_morphemes.push_back(morpheme);
  }
  inline void add_morphemes(std::vector<Morpheme>::iterator begin, std::vector<Morpheme>::iterator end) {
    this->m_morphemes.insert(this->m_morphemes.end(), begin, end);
  }
  inline const std::vector<Morpheme>* get_morphemes() const {
    return &this->m_morphemes;
  }
  inline void reset(){
    this->m_morphemes.clear();
  }
private:
  std::vector<Morpheme> m_morphemes;
};


class Recognition
{
  
public:

  static const unsigned int frames_per_second;
  
  Recognition() {
    pthread_mutex_init(&this->m_lock, NULL);
  }
  
  ~Recognition() {
    pthread_mutex_destroy(&this->m_lock);
  }
  
  inline void lock() {
    pthread_mutex_lock(&this->m_lock);
  }
  
  inline void unlock() {
    pthread_mutex_unlock(&this->m_lock);
  }
  
  inline void set_hypothesis(const Hypothesis &hypo) {
    this->m_hypothesis = hypo;
  }
  
  inline const Hypothesis& get_hypothesis() const {
    return this->m_hypothesis;
  }
  
  inline void set_recognition_frame(unsigned long frame) {
    this->m_recognition_frame = frame;
  }
  
  inline unsigned long get_recognition_frame() const {
    return this->m_recognition_frame;
  }

  inline void add_recognition(const Morpheme &morpheme) {
    this->m_recognized.push_back(morpheme);
  }
  inline void add_recognitions(std::vector<Morpheme>::iterator begin, std::vector<Morpheme>::iterator end) {
    this->m_recognized.insert(this->m_recognized.end(), begin, end);
  }
  inline const std::vector<Morpheme>* get_morphemes() const {
    return &this->m_recognized;
  }
  
  std::string get_recognition_text() const;
  void reset();
  
private:

  Hypothesis m_hypothesis;
  std::vector<Morpheme> m_recognized;
  unsigned long m_recognition_frame;
  pthread_mutex_t m_lock;
};

#endif /*RECOGNITION_HH_*/
