
#include "WindowWaitRecognizer.hh"

WindowWaitRecognizer::WindowWaitRecognizer(const PG_Widget *parent,
                                           const std::string &title,
                                           unsigned int width,
                                           unsigned int height,
                                           msg::InQueue *in_queue)
  : WindowChild(parent, title, width, height, false)
{
  this->m_in_queue = in_queue;
}

void
WindowWaitRecognizer::do_opening()
{
  // If no in queue, no need to wait for recognizer.
  if (!this->m_in_queue)
    this->end_running(1);
//  else
//    fprintf(stderr, "WindowWaitRecognizer::do_opening: "
//                    "Waiting recognizer to be ready...\n");
}

void
WindowWaitRecognizer::handle_broken_pipe()
{
  this->end_running(-1);
}

void
WindowWaitRecognizer::do_running()// throw(msg::ExceptionBrokenPipe)
{
//  fprintf(stderr, "WindowWaitRecognizer 1\n");
  if (this->m_in_queue->get_eof()) {
    fprintf(stderr, "Warning: WindowWaitRecognizer got eof from input!\n");
    this->handle_broken_pipe();
    return;
    //throw msg::ExceptionBrokenPipe(this->m_in_queue->get_fd());
  }
  
//  fprintf(stderr, "WindowWaitRecognizer 2\n");
  this->m_in_queue->flush();
  if (!this->m_in_queue->empty()) {
    msg::Message message = this->m_in_queue->queue.front();
    if (message.type() == msg::M_READY) {
//      fprintf(stderr, "WindowWaitRecognizer::do_running: Recognizer ready.\n");
      this->end_running(1);
    }
    this->m_in_queue->queue.pop_front();
  }
//  fprintf(stderr, "WindowWaitRecognizer 3\n");
}

