/**
 ******************************************************************************
 * Elemental Forms : a lightweight user interface framework                   *
 ******************************************************************************
 * Copyright 2015 Ben Vanik. All rights reserved. Licensed as BSD 3-clause.   *
 * Portions �2011-2015 Emil Seger�s: https://github.com/fruxo/turbobadger     *
 ******************************************************************************
 */

#ifndef EL_ELEMENTS_RESIZER_H_
#define EL_ELEMENTS_RESIZER_H_

#include "el/element.h"

namespace el {
namespace elements {

// A lower right corner resize grip.
// It will resize its parent element.
class Resizer : public Element {
 public:
  TBOBJECT_SUBCLASS(Resizer, Element);
  static void RegisterInflater();

  Resizer();

  HitStatus GetHitStatus(int x, int y) override;
  bool OnEvent(const Event& ev) override;
};

}  // namespace elements
}  // namespace el

#endif  // EL_ELEMENTS_RESIZER_H_
