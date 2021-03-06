/**
 ******************************************************************************
 * Elemental Forms : a lightweight user interface framework                   *
 ******************************************************************************
 * Copyright 2015 Ben Vanik. All rights reserved. Licensed as BSD 3-clause.   *
 * Portions �2011-2015 Emil Seger�s: https://github.com/fruxo/turbobadger     *
 ******************************************************************************
 */

#include <algorithm>

#include "el/animation_manager.h"
#include "el/util/metrics.h"

namespace el {

util::IntrusiveList<Animation> AnimationManager::animating_objects;
int AnimationManager::block_animations_counter = 0;

inline float SmoothStep(float x) { return x * x * (3.0f - 2.0f * x); }

inline float sc(float x) {
  float s = x < 0 ? -1.f : 1.f;
  x = std::abs(x);
  if (x >= 1) return s;
  return s * (x < 0 ? x / 0.5f : (x / (1 + x * x)) / 0.5f);
}

inline float SmoothCurve(float x, float a) {
  float r = a * x / (2 * a * x - a - x + 1);
  r = (r - 0.5f) * 2;
  return sc(r) * 0.5f + 0.5f;
}

// static
void AnimationManager::AbortAllAnimations() {
  while (Animation* obj = animating_objects.GetFirst()) {
    AbortAnimation(obj, true);
  }
}

// static
void AnimationManager::Update() {
  uint64_t time_now = util::GetTimeMS();

  auto iter = animating_objects.IterateForward();
  while (Animation* obj = iter.GetAndStep()) {
    // Adjust the start time if it's the first update time for this object.
    if (obj->adjust_start_time) {
      obj->animation_start_time = time_now;
      obj->adjust_start_time = false;
    }

    // Calculate current progress
    // If animation_duration is 0, it should just complete immediately.
    float progress = 1.0f;
    if (obj->animation_duration != 0) {
      progress = static_cast<float>(time_now - obj->animation_start_time) /
                 static_cast<float>(obj->animation_duration);
      progress = std::min(progress, 1.0f);
    }

    // Apply animation curve
    float tmp;
    switch (obj->animation_curve) {
      case AnimationCurve::kSlowDown:
        tmp = 1 - progress;
        progress = 1 - tmp * tmp * tmp;
        break;
      case AnimationCurve::kSpeedUp:
        progress = progress * progress * progress;
        break;
      case AnimationCurve::kBezier:
        progress = SmoothStep(progress);
        break;
      case AnimationCurve::kSmooth:
        progress = SmoothCurve(progress, 0.6f);
        break;
      default:  // linear (progress is already linear)
        break;
    }

    // Update animation
    obj->InvokeOnAnimationUpdate(progress);
    if (!animating_objects.ContainsLink(obj)) {
      continue;
    }

    // Remove completed animations
    if (progress == 1.0f) {
      animating_objects.Remove(obj);
      obj->InvokeOnAnimationStop(false);
      delete obj;
    }
  }
}

// static
bool AnimationManager::has_running_animations() {
  return animating_objects.HasLinks();
}

// static
void AnimationManager::StartAnimation(Animation* obj,
                                      AnimationCurve animation_curve,
                                      uint64_t animation_duration,
                                      AnimationTime animation_time) {
  if (obj->is_animating()) {
    AbortAnimation(obj, false);
  }
  if (is_animation_blocked()) {
    animation_duration = 0;
  }
  obj->adjust_start_time = animation_time == AnimationTime::kFirstUpdate;
  obj->animation_start_time = util::GetTimeMS();
  obj->animation_duration = std::max(animation_duration, uint64_t(0));
  obj->animation_curve = animation_curve;
  animating_objects.AddLast(obj);
  obj->InvokeOnAnimationStart();
}

// static
void AnimationManager::AbortAnimation(Animation* obj, bool delete_animation) {
  if (obj->is_animating()) {
    animating_objects.Remove(obj);
    obj->InvokeOnAnimationStop(true);
    if (delete_animation) {
      delete obj;
    }
  }
}

// static
bool AnimationManager::is_animation_blocked() {
  return block_animations_counter > 0;
}

// static
void AnimationManager::BeginBlockAnimations() { block_animations_counter++; }

// static
void AnimationManager::EndBlockAnimations() {
  assert(block_animations_counter > 0);
  block_animations_counter--;
}

}  // namespace el
