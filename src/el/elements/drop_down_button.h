/**
 ******************************************************************************
 * Elemental Forms : a lightweight user interface framework                   *
 ******************************************************************************
 * Copyright 2015 Ben Vanik. All rights reserved. Licensed as BSD 3-clause.   *
 * Portions �2011-2015 Emil Seger�s: https://github.com/fruxo/turbobadger     *
 ******************************************************************************
 */

#ifndef EL_ELEMENTS_DROP_DOWN_BUTTON_H_
#define EL_ELEMENTS_DROP_DOWN_BUTTON_H_

#include "el/element.h"
#include "el/elements/form.h"
#include "el/elements/icon_box.h"
#include "el/list_item.h"

namespace el {
namespace elements {

class MenuForm;

// Shows a button that opens a popup with a ListBox with items provided by a
// ItemSource.
class DropDownButton : public Button, public ListItemObserver {
 public:
  TBOBJECT_SUBCLASS(DropDownButton, Button);
  static void RegisterInflater();

  DropDownButton();
  ~DropDownButton() override;

  // Gets the default item source for this element.
  // This source can be used to add items of type GenericStringItem to this
  // element.
  // It is the item source that is fed from resource files.
  // If you need to add other types of items, or if you want to share item
  // sources between several DropDownButton/ListBox elements, use SetSource
  // using a external item source.
  GenericStringItemSource* default_source() { return &m_default_source; }

  int value() override { return m_value; }
  // Sets the selected item.
  void set_value(int value) override;

  // Gets the ID of the selected item, or 0 if there is no item selected.
  TBID selected_item_id();

  // Opens the form if the model has items.
  void OpenForm();

  // Closes the form if it is open.
  void CloseForm();

  // Returns the menu form if it's open, or nullptr.
  MenuForm* menu_if_open() const;

  void OnInflate(const parsing::InflateInfo& info) override;
  bool OnEvent(const Event& ev) override;

  void OnSourceChanged() override;
  void OnItemChanged(size_t index) override;
  void OnItemAdded(size_t index) override {}
  void OnItemRemoved(size_t index) override {}
  void OnAllItemsRemoved() override {}

 protected:
  GenericStringItemSource m_default_source;
  IconBox m_arrow;
  int m_value = -1;
  WeakElementPointer m_form_pointer;  // Dropdown form, if opened.
};

}  // namespace elements
namespace dsl {

struct DropDownButtonNode : public ItemListElementNode<DropDownButtonNode> {
  using R = DropDownButtonNode;
  DropDownButtonNode() : ItemListElementNode("DropDownButton") {}
  //
  R& value(int32_t value) {
    set("value", value);
    return *reinterpret_cast<R*>(this);
  }
};

}  // namespace dsl
}  // namespace el

#endif  // EL_ELEMENTS_DROP_DOWN_BUTTON_H_
