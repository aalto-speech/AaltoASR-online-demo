/*
#include "RecognizerStatus.hh"
#include <stdio.h>

RecognizerStatus::RecognizerStatus()
  : m_recognition_status(READY), m_adaptation_status(NONE)
{
//  fprintf(stderr, "RS constructor\n");
  this->m_adapted = false;
  this->m_was_adapting_when_reseted = false;
}

void
RecognizerStatus::reset_recognition()
{
  this->m_recognition_status = RESETTING;
  this->m_was_adapting_when_reseted = this->m_adaptation_status == ADAPTING;
}

void
RecognizerStatus::set_ready()
{
  if (this->m_recognition_status == RESETTING)
    this->m_recognition_status = READY;

  if (this->m_adaptation_status == ADAPTING && this->m_was_adapting_when_reseted) {
    this->m_adaptation_status = this->m_adapted ? ADAPTED : NONE;
  }
}

void
RecognizerStatus::received_recognition()
{
  if (this->m_recognition_status == READY)
    this->m_recognition_status = RECOGNIZING;
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
  this->m_adaptation_status = NONE;
  this->m_adapted = false;
}

void
RecognizerStatus::recognition_end()
{
  if (this->m_adaptation_status == ADAPTING) {
    this->m_adaptation_status = ADAPTED;
    this->m_adapted = true;
  }
  // When end of audio, be "ready".
  if (this->m_recognition_status == RECOGNIZING)
    this->m_recognition_status = READY;
}

RecognizerStatus::AdaptationStatus
RecognizerStatus::get_adaptation_status() const
{
  return this->m_adaptation_status;
}
//*/
