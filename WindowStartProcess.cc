
#include "WindowStartProcess.hh"
#include "WindowMessageBox.hh"

WindowStartProcess::WindowStartProcess(const PG_Widget *parent,
                                       RecognizerProcess *recognizer)
//                                       Process *process,
//                                       msg::InQueue *in_queue,
//                                       msg::OutQueue *out_queue)
  : WindowWaitRecognizer(parent,
                         "Starting recognizer..",
                         350,
                         150,
                         recognizer ? recognizer->get_in_queue() : NULL)
{
//  this->m_app = app;
//  this->m_process = process;
//  this->m_in_queue = in_queue;
//  this->m_out_queue = out_queue;
  this->m_recognizer = recognizer;
}

void
WindowStartProcess::do_opening()
{
  if (!this->m_recognizer) {
    this->end_running(1);
    return;
  }
  
  // Finish old process and start a new one.
  this->m_recognizer->finish();
  this->m_recognizer->start();
  
/*
  this->finish_process_and_queues();

  this->start_process();
  this->start_queues();
  //*/
  WindowWaitRecognizer::do_opening();
  
  try {
    this->m_recognizer->send_settings();
    this->m_recognizer->get_out_queue()->flush();
//    Settings::send_settings(this->m_out_queue);
  }
  catch (msg::ExceptionBrokenPipe) {
    this->handle_broken_pipe();
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
  if (ret_val == 1) {
    this->do_opening();
  }
  if (ret_val == 2)
    this->end_running(0);
}
/*
void
WindowStartProcess::finish_process_and_queues()
{
  // Finish old process.
  if (this->m_process->is_created()) {
    this->m_process->finish();
    if (this->m_in_queue)
      this->m_in_queue->disable();
    if (this->m_out_queue)
      this->m_out_queue->disable();
  }
}

void
WindowStartProcess::start_process()
{
//  Settings::read_settings();
  if (this->m_process->create() == 0) {
    // All streams must be closed!
//    AudioStream::close_all_streams();
//    AudioStream::close();
//    AudioStream::terminate();
//    this->m_audio_input->terminate();
    // NOTE: Here we should free all memory and close all streams, but how
    // could we do that? If we free everything, we would have to reload
    // surfaces....
//    throw ExceptionChildProcess();

//    Application::app->clean();
//    fcloseall();
    int ret = execlp("ssh",
                     "ssh",
                     Settings::ssh_to.data(),
                     Settings::script_file.data(),
                     (char*)NULL);
                    
    if (ret < 0) {
      perror("Application::run exec() failed");
      exit(1);                                    
    }
    assert(false);

  }
  
  msg::set_non_blocking(this->m_process->read_fd);
  msg::set_non_blocking(this->m_process->write_fd);
}

void
WindowStartProcess::start_queues()
{  
  if (this->m_in_queue) {
    this->m_in_queue->enable(this->m_process->read_fd);
  }
  
  if (this->m_out_queue) {
    this->m_out_queue->enable(this->m_process->write_fd);
  }
}
//*/
