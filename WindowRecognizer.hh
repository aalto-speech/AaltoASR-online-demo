
#ifndef WINDOWRECOGNIZER_HH_
#define WINDOWRECOGNIZER_HH_

#include "Window.hh"
#include "AudioInputController.hh"
#include "WidgetRecognitionArea.hh"
#include "WidgetComparisonArea.hh"
#include "WidgetStatus.hh"
#include "RecognizerListener.hh"
#include "RecognizerProcess.hh"
#include "RecognizerStatus.hh"
#include <pgbutton.h>
#include <pglabel.h>
#include <pgcheckbutton.h>

/** This is the most important gui class which controls the recognition process.
 * It has a few child widgets for taking responsibility of some tasks. */
class WindowRecognizer  :  public Window
{

public:
  
  WindowRecognizer(RecognizerProcess *recognizer);
  virtual ~WindowRecognizer();
  
  virtual void initialize();
  
protected:

  virtual void do_opening();
  virtual void do_running();
  virtual void do_closing(int return_value);
  
  /** Calculates the rect from the top part of the window according to the
   * given indexes.
   * \param column_index Column.
   * \param row_index Row.
   * \return The rectangle area of the window. */
  PG_Rect calculate_button_rect(unsigned int column_index,
                                unsigned int row_index);

  /** A function to automate the creation of ParaGUI buttons.
   * \param label Text of the button.
   * \param column_index Column of the button.
   * \param row_index Row of the button.
   * \param callback Callback function for the button.
   * \return The created button. */
  PG_Button* construct_button(const std::string &label,
                              unsigned int column_index,
                              unsigned int row_index,
                              const SigC::Slot0<bool> &callback);
  
  /** Sends pause or unpause message to recognizer. 
   * \param enable true to unpause, false to pause. */
  void enable_recognizer(bool enable);
  
  // Callback functions for ParaGUI buttons.
  bool handle_stop_button(); //!< Stop audio.
  bool handle_record_button(); //!< Reset recognizer and start recording.
  bool handle_recognize_button();
  bool handle_pause_button();
  bool handle_exit_button();
  bool handle_resetrecog_button();
  bool handle_pauserecog_button();
  bool handle_settings_button();
  bool handle_adaptation_button();
  bool handle_resetadaptation_button();
  bool handle_save_button();
  bool handle_open_button();
  bool handle_showadvanced_button();
  bool handle_key_event(const SDL_KeyboardEvent *key);
  
  /** Sets Pause button pressed (or unpressed) and calls the callback function.
   * \param pause Whether to pause or unpause. */
  void pause_audio_input(bool pause);
  /** Sends reset message to recognizer, and optionally may also clear all
   * audio data.
   * \param reset_audio True if audio data should be cleared too. */
  void reset(bool reset_audio);
  /** Sends end audio message to recognizer and sets the gui according to that
   * state. */
  void end_of_audio();
  
  /** Flushes the out queue to recognizer. Only this function should be used to
   * flush the queue, because this has a proper broken pipe handling. */
  void flush_out_queue();
  
  /** Disables or enables the window, so it won't disturb e.g. child window.
   * \param pause true to disable, false to enable.*/
  virtual void pause_window_functionality(bool pause);
  
  /** Handles broken pipe. Shows error message of the broken pipe and tries to
   * restart the recognition process. If restart fails, sends quit signal.
   * \return false if failed to restart. */
  bool handle_broken_pipe();

  RecognizerProcess *m_recog_proc; //!< Process and pipes to recognizer.
  RecognizerStatus m_recog_status; //!< Recognition and status.
  RecognizerListener m_recog_listener; //!< In queue parser.

  /** Widget which contains time axis, wave and spectrogram views, recognition
   * text, scroll bar and autoscroll radios. */
  WidgetRecognitionArea *m_recognition_area;
  /** Widget which contains reference and hypothesis text fields and some
   * buttons for handling them. */
  WidgetComparisonArea *m_comparison_area;
  /** Widget which contains status texts for recognition and adaptation. */
  WidgetStatus *m_status_bar;
  
private:

  /** Some gui buttons in the top part of the window. */
  PG_Button *m_record_button;
  PG_Button *m_recognize_button;
  PG_Button *m_stop1_button; // Both stop buttons are identical, they just 
  PG_Button *m_stop2_button; // have different location.
  PG_Button *m_pause_button;
  PG_Button *m_showadvanced_button;
  PG_Button *m_adapt_button;
  PG_Button *m_resetadapt_button;
  PG_Button *m_pauserecog_button;
  PG_Button *m_reset_button;

  /** Flag to indicate that recognizer process is disconnected. */
  bool m_broken_pipe;
  
  /** Class to handle audio input and output. Sends the audio messages to
   * out queue. Handles playback also. */
  AudioInputController *m_audio_input;
};

#endif /*WINDOWRECOGNIZER_HH_*/
