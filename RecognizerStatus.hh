
#ifndef RECOGNIZERSTATUS_HH_
#define RECOGNIZERSTATUS_HH_

/** TODO: This class is a very fast implementation to add some new features. It
 * should be refactored into the architecture.
 * TODO: This class should be locked because it used by two threads. */
class RecognizerStatus
{

public:

  RecognizerStatus();

  enum RecognitionStatus { READY, RECOGNIZING, RESETTING };
  enum AdaptationStatus { NONE, ADAPTING, ADAPTED };

  // recog->reset, reset<->ready, ready->recog
  void reset_recognition();
  void set_ready();
  void received_recognition();
  RecognitionStatus get_recognition_status() const;

  void start_adapting();
  void reset_adaptation();
  void recognition_end();
  AdaptationStatus get_adaptation_status() const;
  
private:

  RecognitionStatus m_recognition_status;
  AdaptationStatus m_adaptation_status;
  bool m_adapted;
  bool m_was_adapting_when_reseted;
  
};

#endif /*RECOGNIZERSTATUS_HH_*/
