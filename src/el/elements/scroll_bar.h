/**
 ******************************************************************************
 * Elemental Forms : a lightweight user interface framework                   *
 ******************************************************************************
 * Copyright 2015 Ben Vanik. All rights reserved. Licensed as BSD 3-clause.   *
 * Portions �2011-2015 Emil Seger�s: https://github.com/fruxo/turbobadger     *
 ******************************************************************************
 */

#ifndef EL_ELEMENTS_SCROLL_BAR_H_
#define EL_ELEMENTS_SCROLL_BAR_H_

#include "el/element.h"

namespace el {
namespace elements {

// A scroll bar in the given axis.
class ScrollBar : public Element {
 public:
  TBOBJECT_SUBCLASS(ScrollBar, Element);
  static void RegisterInflater();

  ScrollBar();
  ~ScrollBar() override;

  Axis axis() const override { return m_axis; }
  // Sets along which axis the scrollbar should scroll.
  void set_axis(Axis axis) override;

  double min_value() const { return m_min; }
  double max_value() const { return m_max; }
  // Sets the min, max limits for the scrollbar.
  // The visible parameter is how much of the range that is visible.
  // When this is called, the scrollbar might change value and invoke if the
  // current value is outside of the new limits.
  void set_limits(double min, double max, double visible_range);

  // Returns true if the scrollbar has anywhere to go with the current limits.
  bool can_scroll() const { return m_visible > 0; }

  // Returns true if the scrollbar can scroll in the positive direction with its
  // current limits.
  bool can_scroll_positive() const { return m_value < m_max; }

  // Returns true if the scrollbar can scroll in the negative direction with its
  // current limits.
  bool can_scroll_negative() const { return m_value > m_min; }

  double visible_range() const { return m_visible; }

  double double_value() override { return m_value; }
  // Same as SetValue, but with double precision.
  void set_double_value(double value) override;

  int value() override { return static_cast<int>(double_value()); }
  void set_value(int value) override { set_double_value(value); }

  void OnInflate(const parsing::InflateInfo& info) override;
  bool OnEvent(const Event& ev) override;
  void OnResized(int old_w, int old_h) override;

 protected:
  void UpdateHandle();

  Element m_handle;
  Axis m_axis;
  double m_value = 0;
  double m_min = 0;
  double m_max = 1;
  double m_visible = 1;
  double m_to_pixel_factor = 0;
};

}  // namespace elements
namespace dsl {

struct ScrollBarNode : public ElementNode<ScrollBarNode> {
  using R = ScrollBarNode;
  ScrollBarNode() : ElementNode("ScrollBar") {}
  //
  R& value(float value) {
    set("value", value);
    return *reinterpret_cast<R*>(this);
  }
  //
  R& axis(Axis value) {
    set("axis", el::to_string(value));
    return *reinterpret_cast<R*>(this);
  }
};

}  // namespace dsl
}  // namespace el

#endif  // EL_ELEMENTS_SCROLL_BAR_H_
