/**
 ******************************************************************************
 * Elemental Forms : a lightweight user interface framework                   *
 ******************************************************************************
 * Copyright 2015 Ben Vanik. All rights reserved. Licensed as BSD 3-clause.   *
 * Portions ©2011-2015 Emil Segerås: https://github.com/fruxo/turbobadger     *
 ******************************************************************************
 */

#include <cassert>

#include "el/text/font_effect.h"
#include "el/text/font_face.h"

namespace el {
namespace text {

void BlurGlyph(unsigned char* src, int srcw, int srch, int srcStride,
               unsigned char* dst, int dstw, int dsth, int dstStride,
               float* temp, float* kernel, int kernelRadius) {
  for (int y = 0; y < srch; y++) {
    for (int x = 0; x < dstw; x++) {
      float val = 0;
      for (int k_ofs = -kernelRadius; k_ofs <= kernelRadius; k_ofs++) {
        if (x - kernelRadius + k_ofs >= 0 && x - kernelRadius + k_ofs < srcw) {
          val += src[y * srcStride + x - kernelRadius + k_ofs] *
                 kernel[k_ofs + kernelRadius];
        }
      }
      temp[y * dstw + x] = val;
    }
  }
  for (int y = 0; y < dsth; y++) {
    for (int x = 0; x < dstw; x++) {
      float val = 0;
      for (int k_ofs = -kernelRadius; k_ofs <= kernelRadius; k_ofs++) {
        if (y - kernelRadius + k_ofs >= 0 && y - kernelRadius + k_ofs < srch) {
          val += temp[(y - kernelRadius + k_ofs) * dstw + x] *
                 kernel[k_ofs + kernelRadius];
        }
      }
      dst[y * dstStride + x] = (unsigned char)(val + 0.5f);
    }
  }
}

FontEffect::FontEffect() = default;

FontEffect::~FontEffect() {
  delete[] m_tempBuffer;
  delete[] m_kernel;
}

void FontEffect::SetBlurRadius(int blur_radius) {
  assert(blur_radius >= 0);
  if (m_blur_radius == blur_radius) {
    return;
  }
  m_blur_radius = blur_radius;
  if (m_blur_radius > 0) {
    delete[] m_kernel;
    m_kernel = new float[m_blur_radius * 2 + 1];
    float stdDevSq2 = static_cast<float>(m_blur_radius) / 2.f;
    stdDevSq2 = 2.f * stdDevSq2 * stdDevSq2;
    float scale = 1.f / sqrt(3.1415f * stdDevSq2);
    float sum = 0;
    for (int k = 0; k < 2 * m_blur_radius + 1; k++) {
      float x = static_cast<float>(k - m_blur_radius);
      float kval = scale * exp(-(x * x / stdDevSq2));
      m_kernel[k] = kval;
      sum += kval;
    }
    for (int k = 0; k < 2 * m_blur_radius + 1; k++) {
      m_kernel[k] /= sum;
    }
  }
}

FontGlyphData* FontEffect::Render(GlyphMetrics* metrics,
                                  const FontGlyphData* src) {
  FontGlyphData* effect_glyph_data = nullptr;
  if (m_blur_radius > 0 && src->data8) {
    // Create a new FontGlyphData for the blurred glyph.
    effect_glyph_data = new FontGlyphData;
    if (!effect_glyph_data) {
      return nullptr;
    }
    effect_glyph_data->w = src->w + m_blur_radius * 2;
    effect_glyph_data->h = src->h + m_blur_radius * 2;
    effect_glyph_data->stride = effect_glyph_data->w;
    effect_glyph_data->data8 =
        new unsigned char[effect_glyph_data->w * effect_glyph_data->h];

    // Reserve memory needed for blurring.
    m_blur_temp.Reserve(effect_glyph_data->w * effect_glyph_data->h *
                        sizeof(float));

    // Blur!
    BlurGlyph(src->data8, src->w, src->h, src->stride, effect_glyph_data->data8,
              effect_glyph_data->w, effect_glyph_data->h, effect_glyph_data->w,
              reinterpret_cast<float*>(m_blur_temp.data()), m_kernel,
              m_blur_radius);

    // Adjust glyph position to compensate for larger size.
    metrics->x -= m_blur_radius;
    metrics->y -= m_blur_radius;
  }
  return effect_glyph_data;
}

}  // namespace text
}  // namespace el
