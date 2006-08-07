
#include "WindowReset.hh"

WindowReset::WindowReset(const PG_Widget *parent,
                         msg::InQueue *in_queue,
                         msg::OutQueue *out_queue)
  : WindowWaitRecognizer(parent, "Resetting...", 200, 100, in_queue)
{
  this->m_out_queue = out_queue;
}

void
WindowReset::do_opening()
{
  if (this->m_out_queue) {
    msg::Message message(msg::M_RESET);
    message.set_urgent(true);
    this->m_out_queue->clear();
    this->m_out_queue->add_message(message);
    // Note: this can throw msg::ExceptionBrokenPipe
    try {
      this->m_out_queue->flush();
    }
    catch (msg::ExceptionBrokenPipe) {
      this->end_running(-1);
      return;
    }
  }
  WindowWaitRecognizer::do_opening();
}
