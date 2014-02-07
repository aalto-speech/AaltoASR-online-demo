
#include "WidgetStatus.hh"

WidgetStatus::WidgetStatus(PG_Widget *parent,
                           const PG_Rect &rect,
                           const RecognizerStatus *recog_status)
  : PG_Widget(parent, rect, false),
    m_recog_status(recog_status),
    m_recognition_status(RecognizerStatus::READY),
    m_adaptation_status(RecognizerStatus::NONE)
{
  this->m_recognition_label = new PG_Label(this,
                                           PG_Rect(0, 0, rect.w / 2, rect.h));
  this->m_adaptation_label = new PG_Label(this,
                                          PG_Rect(rect.w / 2, 0, rect.w / 2, rect.h));

  this->m_recognition_label->SetText("Recognizer status: Ready");
  this->m_adaptation_label->SetText("Adaptation status: None");
}
  
void
WidgetStatus::update()
{
  std::string rec_text, ada_text;
  RecognizerStatus::RecognitionStatus rec_stat = this->m_recog_status->get_recognition_status();
  RecognizerStatus::AdaptationStatus ada_stat = this->m_recog_status->get_adaptation_status();

  if (rec_stat != this->m_recognition_status) {
    switch (rec_stat) {
    case RecognizerStatus::READY:
      rec_text = "Recognizer status: Ready";
      break;
    case RecognizerStatus::RECOGNIZING:
      rec_text = "Recognizer status: Recognizing";
      break;
    case RecognizerStatus::RESETTING:
      rec_text = "Recognizer status: Resetting";
      break;
    }
    this->m_recognition_label->SetText(rec_text.c_str());
    this->m_recognition_status = rec_stat;
  }
  
  if (ada_stat != this->m_adaptation_status) {
    switch (ada_stat) {
    case RecognizerStatus::NONE:
      ada_text = "Adaptation status: None";
      break;
    case RecognizerStatus::ADAPTING:
      ada_text = "Adaptation status: Adapting";
      break;
    case RecognizerStatus::ADAPTED:
      ada_text = "Adaptation status: Adapted";
      break;
    }
    this->m_adaptation_label->SetText(ada_text.c_str());
    this->m_adaptation_status = ada_stat;
  }
}
