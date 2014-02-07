
#include "WindowStartProcess.hh"
#include "WindowMessageBox.hh"

WindowStartProcess::WindowStartProcess(const PG_Widget *parent,
                                       RecognizerProcess *recognizer)
  : WindowChild(parent, "Starting recognizer..", 350, 150, false)
{
  this->m_recognizer = recognizer;
}

void
WindowStartProcess::do_opening()
{
  // If no recognizer process, we can just close the window.
  if (!this->m_recognizer) {
    this->end_running(1);
    return;
  }
  
  // Finish old process and start a new one.
  this->m_recognizer->finish();
  this->m_recognizer->start();
  
  WindowChild::do_opening();
  
  try {
    this->m_recognizer->send_settings();
    this->m_recognizer->get_out_queue()->flush();
  }
  catch (msg::ExceptionBrokenPipe) {
    this->handle_broken_pipe();
  }
}

void
WindowStartProcess::do_running()
{
  // Check if pipe is broken.
  if (this->m_recognizer->get_in_queue()->get_eof()) {
    fprintf(stderr, "Warning: WindowWaitRecognizer got eof from input!\n");
    this->handle_broken_pipe();
    return;
  }
  
  // Listen for M_READY message.
  this->m_recognizer->get_in_queue()->flush();
  if (!this->m_recognizer->get_in_queue()->empty()) {
    msg::Message message = this->m_recognizer->get_in_queue()->queue.front();
    if (message.type() == msg::M_READY) {
      this->end_running(1);
    }
    this->m_recognizer->get_in_queue()->queue.pop_front();
  }
}


void
WindowStartProcess::handle_broken_pipe()
{
  WindowMessageBox message(this->m_window,
                           "Error",
                           "Recognizer process couldn't be started.\nDo you want to retry or exit?",
                           "Retry",
                           "Exit");
  message.initialize();
  int ret_val = this->run_child_window(&message);
  
  // On Retry, do the opening procedures again.
  if (ret_val == 1)
    this->do_opening();
  
  // Exiting is done closing the window with signal 0.
  if (ret_val == 2)
    this->end_running(0);
}
