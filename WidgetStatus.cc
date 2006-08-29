
#include "WidgetStatus.hh"

WidgetStatus::WidgetStatus(PG_Widget *parent,
                           const PG_Rect &rect,
                           const RecognizerStatus *recog_status)
  : PG_Widget(parent, rect, false),
    m_recog_status(recog_status)
{
  this->m_recognition_label = new PG_Label(this,
                                           PG_Rect(0, 0, rect.w / 2, rect.h));
  this->m_adaptation_label = new PG_Label(this,
                                          PG_Rect(rect.w / 2, 0, rect.w / 2, rect.h));
}
  
void
WidgetStatus::update()
{
  std::string rec_stat, ada_stat;
  switch (this->m_recog_status->get_recognition_status()) {
  case RecognizerStatus::READY:
    rec_stat = "Recognizer status: Ready";
    break;
  case RecognizerStatus::RECOGNIZING:
    rec_stat = "Recognizer status: Recognizing";
    break;
  case RecognizerStatus::RESETTING:
    rec_stat = "Recognizer status: Resetting";
    break;
  }
  switch (this->m_recog_status->get_adaptation_status()) {
  case RecognizerStatus::NONE:
    ada_stat = "Adaptation status: None";
    break;
  case RecognizerStatus::ADAPTING:
    ada_stat = "Adaptation status: Adapting";
    break;
  case RecognizerStatus::ADAPTED:
    ada_stat = "Adaptation status: Adapted";
    break;
  }
  this->m_recognition_label->SetText(rec_stat.c_str());
  this->m_adaptation_label->SetText(ada_stat.c_str());
}
