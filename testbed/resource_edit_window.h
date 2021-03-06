/**
 ******************************************************************************
 * Elemental Forms : a lightweight user interface framework                   *
 ******************************************************************************
 * �2015 Ben Vanik. All rights reserved. Released under the BSD license.      *
 * Portions �2011-2015 Emil Seger�s: https://github.com/fruxo/turbobadger     *
 ******************************************************************************
 */

#ifndef TESTBED_RESOURCE_EDIT_WINDOW_H_
#define TESTBED_RESOURCE_EDIT_WINDOW_H_

#include "el/element.h"
#include "el/element_listener.h"
#include "el/elements/form.h"
#include "el/elements/list_box.h"
#include "el/elements/text_box.h"
#include "el/message_handler.h"

namespace testbed {

using namespace el;

class ResourceItem : public el::GenericStringItem {
 public:
  ResourceItem(Element* element, const std::string& str);
  Element* GetElement() { return m_element; }

 private:
  Element* m_element;
};

class ResourceEditWindow : public elements::Form,
                           public MessageHandler,
                           public ElementListener {
 public:
  TBOBJECT_SUBCLASS(ResourceEditWindow, elements::Form);

  ResourceEditWindow();
  ~ResourceEditWindow() override;

  void UpdateElementList(bool immediately);

  struct ITEM_INFO {
    ResourceItem* item;
    int index;
  };
  ITEM_INFO GetItemFromElement(Element* element);
  Element* GetSelectedElement() { return m_selected_element.get(); }
  void SetSelectedElement(Element* element);

  void Load(const char* resource_file);
  void RefreshFromSource();

  // == Window
  // ======================================================================
  virtual bool OnEvent(const Event& ev);
  virtual void OnPaintChildren(const PaintProps& paint_props);

  // == MessageHandler
  // ==============================================================
  virtual void OnMessageReceived(Message* msg);

  // == ElementListener
  // ========================================================
  virtual bool OnElementInvokeEvent(Element* element, const Event& ev);
  virtual void OnElementAdded(Element* parent, Element* child);
  virtual void OnElementRemove(Element* parent, Element* child);

 private:
  elements::ListBox* m_element_list;
  ListItemSourceList<ResourceItem> m_element_list_source;
  elements::ScrollContainer* m_scroll_container;
  Element* m_build_container;
  elements::TextBox* m_source_text_box;
  std::string m_resource_filename;
  WeakElementPointer m_selected_element;
  void AddElementListItemsRecursive(Element* element, int depth);
  bool OnDropFileEvent(const Event& ev);
};

}  // namespace testbed

#endif  // TESTBED_RESOURCE_EDIT_WINDOW_H_
