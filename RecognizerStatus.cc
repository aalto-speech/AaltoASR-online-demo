
#include "RecognizerStatus.hh"
#include "str.hh"
#include <stdio.h>
#include <iostream>

const unsigned int RecognizerStatus::frames_per_second = 125;
bool RecognizerStatus::words = false;



RecognizerStatus::RecognizerStatus()
  : m_recognition_status(READY),
    m_adaptation_status(NONE),
    m_message_result_true_called(false)
{
  this->m_adapted = false;
  this->m_was_adapting_when_reseted = false;
  pthread_mutex_init(&this->m_lock, NULL);
}

RecognizerStatus::~RecognizerStatus()
{
  pthread_mutex_destroy(&this->m_lock);
}

void
RecognizerStatus::reset_recognition()
{
  this->lock();
  this->m_recognition_status = RESETTING;
  this->m_was_adapting_when_reseted = this->m_adaptation_status == ADAPTING;
  this->unlock();
}

void
RecognizerStatus::set_ready()
{
  this->lock();
  if (this->m_recognition_status == RESETTING)
    this->m_recognition_status = READY;

  if (this->m_adaptation_status == ADAPTING && this->m_was_adapting_when_reseted) {
    this->m_adaptation_status = this->m_adapted ? ADAPTED : NONE;
  }
  this->unlock();
}

void
RecognizerStatus::received_recognition()
{
  this->lock();
  if (this->m_recognition_status == READY)
    this->m_recognition_status = RECOGNIZING;
  this->unlock();
}

RecognizerStatus::RecognitionStatus
RecognizerStatus::get_recognition_status() const
{
  return this->m_recognition_status;
}

void
RecognizerStatus::start_adapting()
{
  this->m_adaptation_status = ADAPTING;
}

void
RecognizerStatus::reset_adaptation()
{
  this->lock();
  this->m_adapted = false;
  if (this->m_adaptation_status != ADAPTING)
    this->m_adaptation_status = NONE;
  this->unlock();
}

void
RecognizerStatus::recognition_end()
{
  this->lock();
  if (this->m_adaptation_status == ADAPTING) {
    this->m_adaptation_status = ADAPTED;
    this->m_adapted = true;
  }
  // When end of audio, be "ready".
  if (this->m_recognition_status == RECOGNIZING)
    this->m_recognition_status = READY;
  this->unlock();
}

RecognizerStatus::AdaptationStatus
RecognizerStatus::get_adaptation_status() const
{
  return this->m_adaptation_status;
}

std::string
RecognizerStatus::get_recognition_text() const
{
  std::string text;
  RecognizerStatus::append_morphemes(text, this->m_recognized);
  RecognizerStatus::append_morphemes(text, this->m_hypothesis);
  return text;
}

void
RecognizerStatus::reset()
{
  this->m_recognized.clear();
  this->m_hypothesis.clear();
  this->m_recognition_frame = 0;
}

void
RecognizerStatus::parse(const std::string &message)
{
  std::vector<std::string> split_vector;
  Morpheme new_morpheme;
  Morpheme *last_morpheme = NULL;
  bool hypothesis_part = false;
  bool next_is_time = true;
  bool ok = true;
  long last_time = 0;

  // Split the text into parts.
  split_vector = str::split(message, " ", true);

  // Clear previous
  this->m_hypothesis.clear();

  // Check if we are in the end of recognition
  if (split_vector.at(0) == "all") {
    this->m_recognized.clear();
    m_message_result_true_called = true;
  }
  if (m_message_result_true_called && split_vector.at(0) == "part")
    m_message_result_true_called = false;
    
  // Format example: "101 jou 120 lu * 130 on 140 jo 148"
  for (unsigned int ind = 1; ind < split_vector.size(); ind++) {
    if (split_vector.at(ind) == "*") {
      hypothesis_part = true;
    }
    else {
      if (next_is_time) {
        if (ind == split_vector.size()-1) {
          long start_time = str::str2long(split_vector.at(ind));
          if (!words && last_morpheme)
            last_morpheme->duration = start_time - last_morpheme->time;
          last_time = start_time;
        }
        else {
          long start_time = str::str2long(split_vector.at(ind));
          ind++;
          long end_time = str::str2long(split_vector.at(ind));
          new_morpheme.time = start_time < 0 ? 0 : start_time;
          if (words) {
            if (start_time > 0 && end_time > start_time)
              new_morpheme.duration = end_time-start_time;
          }
          else {
            if (last_morpheme)
              last_morpheme->duration = start_time - last_morpheme->time;
          } 
        }
        next_is_time = false;
      }
      else {
        this->write_morpheme_data(new_morpheme.data, split_vector.at(ind));
        if (hypothesis_part) {
          if (words && last_morpheme) {
            Morpheme wb;
            wb.time = last_morpheme->time + last_morpheme->duration;
            wb.duration = new_morpheme.time - wb.time;
            wb.data = std::string(" ");
            this->m_hypothesis.push_back(wb);
          }
          this->m_hypothesis.push_back(new_morpheme);
          last_morpheme = &this->m_hypothesis.back();
        }
        else {
          if (words && last_morpheme) {
            Morpheme wb;
            wb.time = last_morpheme->time + last_morpheme->duration;
            wb.duration = new_morpheme.time - wb.time;
            wb.data = std::string(" ");
            this->m_recognized.push_back(wb);
          }
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
RecognizerStatus::write_morpheme_data(std::string &data, const std::string &morpheme)
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
