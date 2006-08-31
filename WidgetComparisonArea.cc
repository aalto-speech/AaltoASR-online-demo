
#include "WidgetComparisonArea.hh"
#include "WindowOpenTextFile.hh"
#include "WindowSaveTextFile.hh"
#include "WindowComparison.hh"
#include "WindowTextEdit.hh"
#include "Exception.hh"
#include "scrap.h"
#include <pglabel.h>

WidgetComparisonArea::WidgetComparisonArea(Window &parent,
                                           const PG_Rect &rect,
                                           RecognizerStatus *recognition)
  : PG_Widget(parent.get_widget(), rect, false),
    m_parent(parent),
    m_recognition(recognition)
{
  
  const unsigned int field_space = 30;
  const unsigned int field_width = (this->my_width - field_space) / 2;
  const unsigned int button_space = 5;
  const unsigned int button_width = 65;

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
  this->m_original_text->SetEditable(false);
  this->m_recognition_text->SetBorderSize(1);
  this->m_recognition_text->SetEditable(false);

  PG_Button *button;
  unsigned int x;
  
  // Buttons for original text field.
  x = (field_width - 5 * button_width - 4 * button_space) / 2;
  button = new PG_Button(this, PG_Rect(x, this->my_height - 50, button_width, 40), "Edit");
  button->sigClick.connect(slot(*this, &WidgetComparisonArea::handle_editoriginal_button));
  
  x += button_width + button_space;
  button = new PG_Button(this, PG_Rect(x, this->my_height - 50, button_width, 40), "Paste");
  button->sigClick.connect(slot(*this, &WidgetComparisonArea::handle_pasteoriginal_button));
  
  x += button_width + button_space;
  button = new PG_Button(this, PG_Rect(x, this->my_height - 50, button_width, 40), "Clear");
  button->sigClick.connect(slot(*this, &WidgetComparisonArea::handle_clearoriginal_button));
  
  x += button_width + button_space;
  button = new PG_Button(this, PG_Rect(x, this->my_height - 50, button_width, 40), "Open");
  button->sigClick.connect(slot(*this, &WidgetComparisonArea::handle_openoriginal_button));
  
  x += button_width + button_space;
  button = new PG_Button(this, PG_Rect(x, this->my_height - 50, button_width, 40), "Save");
  button->sigClick.connect(slot(*this, &WidgetComparisonArea::handle_saveoriginal_button));
  

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

  // Initialize clipboard.
  if (init_scrap() < 0) {
    fprintf(stderr, "WidgetComparisonArea constructor: Failed to initialize clipboard\n");
    throw Exception("Clipboard initialization failed.\n");
  }

}

WidgetComparisonArea::~WidgetComparisonArea()
{
  // Uninitialize clipboard
  lost_scrap();
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
WidgetComparisonArea::handle_editoriginal_button()
{
  //*
  std::string text = this->m_original_text->GetText();
  WindowTextEdit window(this->m_parent.get_widget(), "Edit reference", 300, 400, &text);
  window.initialize();
  this->m_parent.run_child_window(&window);
  this->m_original_text->SetText(text.data());
  this->m_original_text->Update();
  //*/
  return true;
}

bool
WidgetComparisonArea::handle_pasteoriginal_button()
{
  char *scrap = NULL;
  int scraplen = 0;
  get_scrap(T('T','E','X','T'), &scraplen, &scrap);

  // Convert carriage returns into normal new lines because ParaGUI doesn't
  // show carriage returns.
  for (int ind = 0; ind < scraplen; ind++) {
    if ((int)scrap[ind] == 13)
      scrap[ind] = '\n';
  }
  this->m_original_text->SetText(scrap);
  delete[] scrap;
  this->m_original_text->Update();
  return true;
}


bool
WidgetComparisonArea::handle_updaterecognition_button()
{
  this->m_recognition->lock();
  this->m_recognition_text->SetText(this->m_recognition->get_recognition_text().data());
  this->m_recognition->unlock();

  this->m_recognition_text->Update();

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
  WindowComparison window(this->m_parent.get_widget(),
                              this->m_original_text->GetText(),
                              this->m_recognition_text->GetText());
  window.initialize();
  this->m_parent.run_child_window(&window);
  return true;
}
