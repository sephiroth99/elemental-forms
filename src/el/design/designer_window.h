/**
 ******************************************************************************
 * Elemental Forms : a lightweight user interface framework                   *
 ******************************************************************************
 * �2015 Ben Vanik. All rights reserved. Released under the BSD license.      *
 * Portions �2011-2015 Emil Seger�s: https://github.com/fruxo/turbobadger     *
 ******************************************************************************
 */

#ifndef EL_DESIGN_DESIGNER_WINDOW_H_
#define EL_DESIGN_DESIGNER_WINDOW_H_

#include "el/element.h"
#include "el/element_listener.h"
#include "el/elements/list_box.h"
#include "el/elements/text_box.h"
#include "el/event_handler.h"
#include "el/message_handler.h"
#include "el/window.h"

namespace el {
namespace design {

class DesignerWindow : public Window,
                       public MessageHandler,
                       public ElementListener {
 public:
  TBOBJECT_SUBCLASS(DesignerWindow, Window);

  DesignerWindow();
  ~DesignerWindow() override;

  void Show(Element* root_element);

  void BindContent(Element* bind_element);
  void CloseContent();

 private:
  class ElementItem : public GenericStringItem {
   public:
    ElementItem(Element* element, const std::string& str)
        : GenericStringItem(str), element_(element) {}
    Element* element() const { return element_; }

   private:
    Element* element_;
  };

  void RefreshContent();
  void PopulateElementListBox();

  EventHandler event_handler_ = {this};
  elements::ListBox* element_list_box_ = nullptr;
  ListItemSourceList<ElementItem> element_list_source_;
  elements::TextBox* source_text_box_ = nullptr;
  Element* build_container_ = nullptr;
  Element* build_content_root_ = nullptr;

  WeakElementPointer bind_element_;
};

}  // namespace design
}  // namespace el

#endif  // EL_DESIGN_DESIGNER_WINDOW_H_