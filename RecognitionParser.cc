
#include "RecognitionParser.hh"
#include "str.hh"

const unsigned int RecognitionParser::frames_per_second = 125;

RecognitionParser::RecognitionParser()
{
  pthread_mutex_init(&this->m_lock, NULL);
}

RecognitionParser::~RecognitionParser()
{
  pthread_mutex_destroy(&this->m_lock);
}

std::string
RecognitionParser::get_recognition_text() const
{
  std::string text;
  RecognitionParser::append_morphemes(text, this->m_recognized);
  RecognitionParser::append_morphemes(text, this->m_hypothesis);
  return text;
}

void
RecognitionParser::reset()
{
  this->m_recognized.clear();
  this->m_hypothesis.clear();
  this->m_recognition_frame = 0;
}

void
RecognitionParser::parse(const std::string &message)
{
  std::vector<std::string> split_vector;
  Morpheme new_morpheme;
  Morpheme *last_morpheme = NULL;
  bool hypothesis_part = false;
  bool next_is_time = true;
  bool ok = true;
  long last_time = 0;
  
  this->m_hypothesis.clear();

  // Split the text into parts.
  str::split(&message, " ", true, &split_vector);
  
  // Format example: "101 jou 120 lu * 130 on 140 jo 148"
  for (unsigned int ind = 0; ind < split_vector.size(); ind++) {
    if (split_vector.at(ind) == "*") {
      hypothesis_part = true;
    }
    else {
      if (next_is_time) {
        long time = str::str2long(&split_vector.at(ind), &ok);
        new_morpheme.time = time < 0 ? 0 : time;
        if (last_morpheme)
          last_morpheme->duration = time - last_morpheme->time;
        last_time = time;
        next_is_time = false;
      }
      else {
        this->write_morpheme_data(new_morpheme.data, split_vector.at(ind));
        if (hypothesis_part) {
          this->m_hypothesis.push_back(new_morpheme);
          last_morpheme = &this->m_hypothesis.back();
        }
        else {
          this->m_recognized.push_back(new_morpheme);
          last_morpheme = &this->m_recognized.back();
        }
        next_is_time = true;
      }
    }
  }
  
  // Check if error occured in parsing.
  if (!ok)
    fprintf(stderr, "Warning: Recognition did not parse time properly.\n");
  if (next_is_time)
    fprintf(stderr, "Warning: No ending frame in parsed recognition.\n");
  
  // Update last frame.
  this->m_recognition_frame = last_time;
}

void
RecognitionParser::write_morpheme_data(std::string &data, const std::string &morpheme)
{
  if (morpheme == "<w>")
    data = " ";
  else if (/*morpheme == "<s>" || */morpheme == "</s>")
    data = ".";
  else if (morpheme == "<s>")
    data = "";
  else if (morpheme != "<s>")
    data = morpheme;
}
