/**
 ******************************************************************************
 * Elemental Forms : a lightweight user interface framework                   *
 ******************************************************************************
 * �2015 Ben Vanik. All rights reserved. Released under the BSD license.      *
 * Portions �2011-2015 Emil Seger�s: https://github.com/fruxo/turbobadger     *
 ******************************************************************************
 */

#ifndef EL_ELEMENTS_ICON_BOX_H_
#define EL_ELEMENTS_ICON_BOX_H_

#include "el/element.h"

namespace el {
namespace elements {

// A element showing a skin element, constrained in size to its skin.
// If you need to load and show images dynamically (i.e. not always loaded as
// the skin), you can use ImageBox.
class IconBox : public Element {
 public:
  TBOBJECT_SUBCLASS(IconBox, Element);
  static void RegisterInflater();

  IconBox() = default;
  IconBox(const TBID& skin_bg) { set_background_skin(skin_bg); }

  PreferredSize OnCalculatePreferredSize(
      const SizeConstraints& constraints) override;
};

}  // namespace elements
}  // namespace el

#endif  // EL_ELEMENTS_ICON_BOX_H_