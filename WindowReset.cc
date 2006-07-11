
#include "WindowReset.hh"

WindowReset::WindowReset(const PG_Widget *parent, msg::InQueue *in_queue, OutQueueController *out_queue)
  : m_parent(parent)
{
//  this->m_ok_button = NULL;
//  this->m_audio_input = audio_input;
  this->m_parent = parent;
  this->m_in_queue = in_queue;
  this->m_out_queue = out_queue;
}

WindowReset::~WindowReset()
{
//  delete this->m_ok_button;
}
/*
void
WindowReset::initialize()
{
  Window::initialize();
  
  this->m_ok_button = new PG_Button(this->m_window, PG_Rect(10,10,50,50), "OK");
  this->m_window->AddChild(this->m_ok_button);
  this->m_ok_button->sigClick.connect(slot(*this, &WindowReset::button_pressed));
}
//*/
PG_Widget*
WindowReset::create_window()
{
  if (this->m_parent == NULL) {
    fprintf(stderr, "WindowReset::create_window got NULL-parent.\n");
    assert(false);
  }

  Uint16 width = 200;
  Uint16 height = 100;
  Sint16 x = this->m_parent->my_xpos + (this->m_parent->my_width - width) / 2;
  Sint16 y = this->m_parent->my_ypos + (this->m_parent->my_height - height) / 2;

  return new PG_Window(NULL,
                       PG_Rect(x, y, width, height),
                       "Reseting..",
                       PG_Window::MODAL);
}

void
WindowReset::do_opening()
{
  if (this->m_out_queue) {
    this->m_out_queue->reset();
    this->m_out_queue->flush();
  }

  fprintf(stderr, "WindowReset::do_opening: Waiting recognizer to reset...\n");

  // If no in queue, no need to wait for recognizer.
  if (!this->m_in_queue)
    this->close(1);
}

void
WindowReset::do_running()
{
  msg::Message message;
  this->m_in_queue->flush();
  if (!this->m_in_queue->empty()) {
    message = this->m_in_queue->queue.front();
    if (message.type() == msg::M_READY) {
      fprintf(stderr, "WindowReset::do_running: Recognizer ready.\n");
      this->close(1);
    }
    this->m_in_queue->queue.pop_front();
  }
}

/*
bool
WindowReset::button_pressed(PG_Button *button)
{
  if (button == this->m_ok_button) {
    this->close();
  }
  return true;
}
//*/
