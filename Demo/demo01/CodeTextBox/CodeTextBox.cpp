#include "CodeTextBox.h"

#include "tb_widgets_reader.h"
#include <ctype.h>

using namespace tb;
TB_WIDGET_FACTORY(CodeTextBox, Value::Type::kString, WidgetZ::kTop) {}

CodeTextBox::CodeTextBox() : TextBox(), inComment(false) {}

void CodeTextBox::OnInflate(const InflateInfo& info) {
  TextBox::OnInflate(info);
}

void CodeTextBox::DrawString(int32_t x, int32_t y, FontFace* font,
                             const Color& color, const char* str, size_t len) {
  Color finalColor(color);
  StringHasColorOverride(str, len, finalColor);
  TextBox::DrawString(x, y, font, finalColor, str, len);
}

void CodeTextBox::OnBreak() { inComment = false; }

bool CodeTextBox::StringHasColorOverride(const char* str, size_t len,
                                         Color& colour) {
  if (strlen(str) >= 2) {
    if (str[0] == '/' && str[1] == '/') {
      inComment = true;
    }
  }

  if (inComment) {
    colour = Color(113, 143, 113);
    return true;
  }

  char* keywords[] = {"in",  "vec3",  "uvec2",   "const", "uniform", "void",
                      "if",  "float", "vec4",    "for",   "uint",    "abs",
                      "sin", "cos",   "texture", "int"};

  for (int32_t keywordIdx = 0; keywordIdx < 16; keywordIdx++) {
    char* matchAgainst = keywords[keywordIdx];
    int32_t matchLen = (int32_t)strlen(matchAgainst);
    if (matchLen == len) {
      auto matched = true;
      for (int32_t i = 0; i < len; i++) {
        if (toupper(matchAgainst[i]) != toupper(str[i])) {
          matched = false;
          break;
        }
      }

      if (matched) {
        colour = Color(90, 127, 230);
        return true;
      }
    }
  }

  return false;
}