/**
 ******************************************************************************
 * Elemental Forms : a lightweight user interface framework                   *
 ******************************************************************************
 * Copyright 2015 Ben Vanik. All rights reserved. Licensed as BSD 3-clause.   *
 * Portions �2011-2015 Emil Seger�s: https://github.com/fruxo/turbobadger     *
 ******************************************************************************
 */

#ifndef EL_UTIL_DEBUG_H_
#define EL_UTIL_DEBUG_H_

#include <string>

#include "el/config.h"

#ifdef EL_RUNTIME_DEBUG_INFO
void TBDebugOut(const char* str, ...);
inline void TBDebugOut(const std::string& str) { TBDebugOut(str.c_str()); }
#define EL_IF_DEBUG(debug) debug
#else
#define TBDebugOut(str, ...) ((void)0)
#define EL_IF_DEBUG(debug)
#endif  // EL_RUNTIME_DEBUG_INFO

namespace el {
class Element;
}  // namespace el

#ifdef EL_RUNTIME_DEBUG_INFO

namespace el {
namespace util {

class DebugInfo {
 public:
  static DebugInfo* get() { return &debug_info_singleton_; }

  DebugInfo();

  enum class Setting {
    // Show elements bounds.
    kLayoutBounds,
    // Show child element clipping set by some elements.
    kLayoutClipping,
    // Show highlights on elements that recalculate their preferred size, and
    // those who recalculate their layout.
    kLayoutSizing,
    // Show render batch info and log batch info in the debug output. It depends
    // on the renderer backend if this is available.
    kDrawRenderBatches,
    // Render the bitmap fragments of the skin.
    kDrawSkinBitmapFragments,
    // Render the bitmap fragments of the font that's set on the hovered or
    // focused element.
    kDrawFontBitmapFragments,
    // Render the bitmap fragments of the image manager.
    kDrawImageBitmapFragments,

    kSettingCount,
  };
  int settings[static_cast<int>(Setting::kSettingCount)] = {0};

 private:
  static DebugInfo debug_info_singleton_;
};

// Shows a form containing runtime debugging settings.
void ShowDebugInfoSettingsForm(Element* root);

#define EL_DEBUG_SETTING(setting) \
  el::util::DebugInfo::get()->settings[static_cast<int>(setting)]
#define EL_IF_DEBUG_SETTING(setting, code) \
  if (EL_DEBUG_SETTING(setting)) {         \
    code;                                  \
  }

}  // namespace util
}  // namespace el

#else
namespace el {
namespace util {
inline void ShowDebugInfoSettingsForm(Element* root) {}
}  // namespace util
}  // namespace el
#define EL_DEBUG_SETTING(setting) false
#define EL_IF_DEBUG_SETTING(setting, code)
#endif  // EL_RUNTIME_DEBUG_INFO

#endif  // EL_UTIL_DEBUG_H_
