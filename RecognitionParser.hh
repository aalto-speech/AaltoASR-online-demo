
#ifndef RECOGNITION_HH_
#define RECOGNITION_HH_

#include <list>
#include <string>

/** Data structure for morphemes. */
struct Morpheme
{
  unsigned long time; //!< Starting frame of the morpheme.
  unsigned long duration; //!< Duration of the morpheme.
  std::string data; //!< The morpheme.
};

/** Container type for morphemes. */
typedef std::list<Morpheme> MorphemeList;

/** Data structure containing a morpheme lists both for recognized part and
 * hypothesis part. Recognitions are passed as a message to parse function.
 *  */
class RecognitionParser
{
  
public:

  /** Constant telling how many (recognition) frames in second. */
  static const unsigned int frames_per_second;
  
  /** Constructs the object. */
  RecognitionParser();
  /** Destructs the object. */
  ~RecognitionParser();
  
  /**
   * Takes a recognition message as a parameter and updates recognized text
   * and hypothesis according to that.
   * \param message Formatted string containing the information: starting
   *                and ending frames for every morpheme and separation of
   *                hypothesis and recognition by * .
   *                Valid format examples:
   *                  "100 jou 112 lu * 128 on 140 jo 151"
   *                  "120 sana 135"
   *                  "101"
   *                  "* 120 hypo 151 teesi 174"
   * */
  void parse(const std::string &message);
  
  /** Function returns when no other thread has lock on. You can use lock
   * function if you want to synchronize threads.
   * 
   * NOTE: A bit stupid to have locks here: what does this data structure
   *       know about threads. Maybe there's a better way to do this? */
  inline void lock();
  /** Releases the lock for this thread. */
  inline void unlock();
  
  /** \return List of morphemes considered as hypothesis. */
  inline const MorphemeList& get_hypothesis() const;
  
  /** \return List of morphemes considered recognized. */
  inline const MorphemeList& get_recognized() const;

  /** \return The last frame of the whole recognition and hypothesis. */
  inline unsigned long get_recognition_frame() const;

  /** \return Recognized and hypothesis morphemes as a text. */
  std::string get_recognition_text() const;
  
  /** Clears all morphemes and recognition frame. */
  void reset();

protected:
  
  /** Writes morpheme into the string.
   * \param str Writes morphemes into this string.
   * \param morphemes Morphemes to write. */
  inline static void append_morphemes(std::string &str,
                                      const MorphemeList &morphemes);

  /** Copies the morpheme to the data string. Changes "<w> to " ", and ignores
   * "<s>" or "</s>". TODO: Should sentence breaks be handled in some way?
   * \param data Destination of the copy.
   * \param morpheme Source of the copy. */
  static void write_morpheme_data(std::string &data,
                                  const std::string &morpheme);
                                  
private:

  MorphemeList m_hypothesis; //!< List of hypothesis morphemes.
  MorphemeList m_recognized; //!< List of recognized morphemes.

  unsigned long m_recognition_frame; //!< Frame set by user.

  pthread_mutex_t m_lock; //!< Lock for users of this class.
  
};

void
RecognitionParser::lock()
{
  pthread_mutex_lock(&this->m_lock);
}

void
RecognitionParser::unlock()
{
  pthread_mutex_unlock(&this->m_lock);
}

const MorphemeList&
RecognitionParser::get_hypothesis() const
{
  return this->m_hypothesis;
}

const MorphemeList&
RecognitionParser::get_recognized() const
{
  return this->m_recognized;
}

unsigned long
RecognitionParser::get_recognition_frame() const
{
  return this->m_recognition_frame;
}

void
RecognitionParser::append_morphemes(std::string &str, const MorphemeList &morphemes)
{
  for (MorphemeList::const_iterator iter = morphemes.begin();
       iter != morphemes.end();
       iter++) {
        
    str.append(iter->data);
  }
}

#endif /*RECOGNITION_HH_*/
