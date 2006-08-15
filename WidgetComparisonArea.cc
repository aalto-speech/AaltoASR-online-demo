
#include "WidgetComparisonArea.hh"
#include "WindowOpenTextFile.hh"
#include "WindowSaveTextFile.hh"
#include "WindowComparison.hh"
#include <pglabel.h>

WidgetComparisonArea::WidgetComparisonArea(Window &parent,
                                           const PG_Rect &rect,
                                           RecognitionParser *recognition)
  : PG_Widget(parent.get_widget(), rect, false),
    m_parent(parent),
    m_recognition(recognition)
{
  
  const unsigned int field_space = 30;
  const unsigned int field_width = (this->my_width - field_space) / 2;
  const unsigned int button_space = 10;
  const unsigned int button_width = 80;

  // Create titles for both fields.  
  PG_Label *label;
  label = new PG_Label(this, PG_Rect(0,0,0,0), "Reference");
  label->SetSizeByText();
  label->MoveWidget((field_width - label->Width()) / 2, 0);

  label = new PG_Label(this, PG_Rect(0,0,0,0), "Hypothesis");
  label->SetSizeByText();
  label->MoveWidget(field_width + field_space + (field_width - label->Width()) / 2, 0);


  // Create text areas for both fields.
  this->m_original_text = new WidgetMultiLineEdit(this,
                                               PG_Rect(0,
                                                       30,
                                                       field_width,
                                                       this->my_height - 90));

  this->m_recognition_text = new WidgetMultiLineEdit(this,
                                                  PG_Rect(field_width + field_space,
                                                          30,
                                                          field_width,
                                                          this->my_height - 90));

  this->m_original_text->SetBorderSize(1);
  this->m_recognition_text->SetBorderSize(1);
  this->m_recognition_text->SetEditable(false);

  PG_Button *button;
  unsigned int x;
  
  // Buttons for original text field.
  x = (field_width - 3 * button_width - 2 * button_space) / 2;
  button = new PG_Button(this, PG_Rect(x, this->my_height - 50, button_width, 40), "Open");
  button->sigClick.connect(slot(*this, &WidgetComparisonArea::handle_openoriginal_button));
  
  x += button_width + button_space;
  button = new PG_Button(this, PG_Rect(x, this->my_height - 50, button_width, 40), "Save");
  button->sigClick.connect(slot(*this, &WidgetComparisonArea::handle_saveoriginal_button));
  
  x += button_width + button_space;
  button = new PG_Button(this, PG_Rect(x, this->my_height - 50, button_width, 40), "Clear");
  button->sigClick.connect(slot(*this, &WidgetComparisonArea::handle_clearoriginal_button));
  

  // Buttons for recognition text field.
  x = (field_width + field_space) + (field_width - 2 * button_width - 1 * button_space) / 2;
  button = new PG_Button(this, PG_Rect(x, this->my_height - 50, button_width, 40), "Update");
  button->sigClick.connect(slot(*this, &WidgetComparisonArea::handle_updaterecognition_button));
  
  x += button_width + button_space;
  button = new PG_Button(this, PG_Rect(x, this->my_height - 50, button_width, 40), "Save");
  button->sigClick.connect(slot(*this, &WidgetComparisonArea::handle_saverecognition_button));
  

  // Button for comparing original and recognized text.
  const unsigned int compare_width = 2 * button_width;
  x = field_width + (field_space - compare_width) / 2;
  button = new PG_Button(this, PG_Rect(x, this->my_height - 50, compare_width, 40), "<- Compare ->");
  button->sigClick.connect(slot(*this, &WidgetComparisonArea::handle_compare_button));

}

bool
WidgetComparisonArea::handle_openoriginal_button()
{
  std::string content;
  WindowOpenTextFile window(this->m_parent.get_widget(), content);
  window.initialize();
  if (this->m_parent.run_child_window(&window) == 1) {
    this->m_original_text->SetText(content.data());
    this->m_original_text->SetVPosition(0);
//    this->m_original_text->Update();
  }
  return true;
}

bool
WidgetComparisonArea::handle_saveoriginal_button()
{
  WindowSaveTextFile window(this->m_parent.get_widget(),
                            this->m_original_text->GetText());
  window.initialize();
  this->m_parent.run_child_window(&window);
  return true;
}

bool
WidgetComparisonArea::handle_clearoriginal_button()
{
  this->m_original_text->SetText("");
//  this->m_original_text->SetVPosition(0);
  this->m_original_text->Update();
  return true;
}

bool
WidgetComparisonArea::handle_updaterecognition_button()
{
//  Uint16 x = this->m_recognition_text->GetScrollPosX();
//  Uint16 y = this->m_recognition_text->GetScrollPosY();
//  int y = this->m_recognition_text->GetVPosition();
//  int y = this->m_recognition_text->GetCursorPos();
  this->m_recognition->lock();
  this->m_recognition_text->SetText(this->m_recognition->get_recognition_text().data());
  this->m_recognition->unlock();
  // Keep old scroll location.
  // NOTE: It was a good idea to keep the old scroll position. But ScrollTo
  // function doesn't work properly: 1) it doesn't move the scroll bar and 2) it
  // updates the screen. This makes the screen flicker and the scroll bar
  // is in different position than the text area. Should overwrite some
  // functions, but it isn't very trivial task, so I won't implement it at
  // this time. Implement your own class derived from RichEdit and write your
  // own ScrollTo function there using ParaGUI open source code as a guide.
  // NEW NOTE: I changed to MultiLineEdit because RichEdit had some bugs in it.
  // MultiLineEdit has a function SetVPosition to set the scroll position.
  // Unfortunately this member function is private so we cannot use it without
  // some "hacking". And SetVPosition also updates the screen, so the
  // flickering problem remains.
//  this->m_recognition_text->ScrollTo(x, y);
//  this->m_recognition_text->SetVPosition(y);
  this->m_recognition_text->Update();
//  this->m_recognition_text->SetCursorPos(y);
  return true;
}

bool
WidgetComparisonArea::handle_saverecognition_button()
{
  WindowSaveTextFile window(this->m_parent.get_widget(),
                            this->m_recognition_text->GetText());
  window.initialize();
  this->m_parent.run_child_window(&window);
  return true;
}

bool
WidgetComparisonArea::handle_compare_button()
{
//  this->m_parent.error("Comparing not yet implemented.", Window::ERROR_NORMAL);
  WindowComparison window(this->m_parent.get_widget(),
                              this->m_original_text->GetText(),
                              this->m_recognition_text->GetText());
  window.initialize();
  this->m_parent.run_child_window(&window);
  return true;
}
