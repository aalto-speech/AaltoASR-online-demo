
#include "WindowInit.hh"

WindowInit::WindowInit(msg::InQueue *in_queue)
{
  this->m_in_queue = in_queue;
  this->m_text = NULL;
}

WindowInit::~WindowInit()
{
  delete this->m_text;
}

void
WindowInit::initialize()
{
  Window::initialize();
  
  this->m_text = new PG_Label(this->m_window, PG_Rect(20,100,200,100), NULL);
  this->m_window->AddChild(this->m_text);
}

void
WindowInit::do_opening()
{
  if (!this->m_in_queue) {
    this->close();
    return;
  }

  this->m_text->SetFontColor(PG_Color(255,255,0));
  this->m_text->SetAlignment(PG_Label::CENTER);
  this->m_text->SetText("Waiting for recognizer...");
}

void
WindowInit::do_running()
{
  msg::Message message;
  
  // Check for ready message
  this->m_in_queue->flush();
  if (!this->m_in_queue->empty()) {
    message = this->m_in_queue->queue.front();
    if (message.type() == msg::M_READY) {
      fprintf(stderr, "Recognizer ready.\n");
      this->m_text->SetText("Recognizer ready.");
      this->close(1);
    }
    else {
      fprintf(stderr, "Invalid ready message from recognizer.\n");
      // Tähän voisi laittaa, että kirjoittaa tekstin näytölle ja odottaa
      // jotain näppäimen painallusta, ja sitten this->quit()
      assert(false);
    }
    this->m_in_queue->queue.pop_front();
  }
}

void
WindowInit::do_closing(int return_value)
{
  // Nothing to do...
}

bool
WindowInit::eventKeyDown(const SDL_KeyboardEvent *key)
{
//  fprintf(stderr, "WindowInit::eventKeyDown\n");
//  this->close();
  return true;
}
