/**
 ******************************************************************************
 * xenia-project/turbobadger : a fork of Turbo Badger for Xenia               *
 ******************************************************************************
 * Copyright 2011-2015 Emil Segerås and Ben Vanik. All rights reserved.       *
 * See tb_core.h and LICENSE in the root for more information.                *
 ******************************************************************************
 */

#ifndef TB_ANIMATION_H_
#define TB_ANIMATION_H_

#include <cstdint>

#include "tb/util/link_list.h"
#include "tb/util/object.h"

namespace tb {

class Animation;

// Defines how the animation progress value is interpolated.
enum class AnimationCurve {
  kLinear,    // Linear.
  kSlowDown,  // Fast start, slow end.
  kSpeedUp,   // Slow start, fast end.
  kBezier,    // Slow start, slow end. Almost linear.
  kSmooth,    // Slow start, slow end. Stronger than AnimationCurve::kBezier.
};

// Defines what the animation duration time is relative to.
enum class AnimationTime {
  // The start time begins when the animation start in
  // AnimationManager::StartAnimation.
  kImmediately,

  // The animation start in StartAnimation just as with
  // AnimationTime::kImmediately, but the start time is adjusted to when the
  // animations Update is about to be called the first time since it was
  // started.
  // Using this is most often preferable since starting a animation is often
  // accompanied with some extra work that might eat up a considerable time of
  // the total duration (and chop of the beginning of it).
  // F.ex: Creating a window and starting its appearance animation. During
  // initialization of the window, you might initiate loading of additional
  // resources. When that is done and you finally end up updating animations,
  // most of the animation time might already have passed. If the animation
  // start time is adjusted to the first update, the whole animation will run
  // from 0.0 - 1.0 smoothly when the initialization is done.
  kFirstUpdate,
};

// Listens to the progress of Animation.
class AnimationListener : public util::TBLinkOf<AnimationListener> {
 public:
  virtual ~AnimationListener() = default;

  // Called after the animation object handled its own OnAnimationStart.
  // See Animation::OnAnimationStart for details.
  virtual void OnAnimationStart(Animation* obj) = 0;

  // Called after the animation object handled its own OnAnimationUpdate.
  // See Animation::OnAnimationUpdate for details.
  virtual void OnAnimationUpdate(Animation* obj, float progress) = 0;

  // Called after the animation object handled its own OnAnimationStop.
  // See Animation::OnAnimationStop for details.
  virtual void OnAnimationStop(Animation* obj, bool aborted) = 0;
};

// Base class for all animated objects.
class Animation : public util::TypedObject, public util::TBLinkOf<Animation> {
 public:
  const static AnimationCurve kDefaultCurve = AnimationCurve::kSlowDown;
  const static uint64_t kDefaultDuration = 200;

  AnimationCurve animation_curve;
  uint64_t animation_start_time;
  uint64_t animation_duration;
  bool adjust_start_time;

 public:
  TBOBJECT_SUBCLASS(Animation, util::TypedObject);

  ~Animation() override = default;

  /** Returns true if the object is currently animating. */
  bool IsAnimating() const { return linklist ? true : false; }

  // Called on animation start.
  virtual void OnAnimationStart() = 0;

  // Called on animation update. progress is current progress from 0 to 1.
  // Note that it isn't called on start, so progress 0 might not happen.
  // It will be called with progress 1 before the animation is completed
  // normally (not aborted).
  virtual void OnAnimationUpdate(float progress) = 0;

  // Called on animation stop. aborted is true if it was aborted before
  // completion.
  // Note that if a animation is started when it's already running, it will
  // first be aborted and then started again.
  virtual void OnAnimationStop(bool aborted) = 0;

  // Adds an listener to this animation object.
  void AddListener(AnimationListener* listener) {
    m_listeners.AddLast(listener);
  }

  // Removes an listener from this animation object.
  void RemoveListener(AnimationListener* listener) {
    m_listeners.Remove(listener);
  }

 private:
  friend class AnimationManager;
  util::TBLinkListOf<AnimationListener> m_listeners;
  void InvokeOnAnimationStart();
  void InvokeOnAnimationUpdate(float progress);
  void InvokeOnAnimationStop(bool aborted);
};

// System class that manages all animated object.
class AnimationManager {
 private:
  static util::TBLinkListOf<Animation> animating_objects;
  static int block_animations_counter;

 public:
  // Updates all running animations.
  static void Update();

  // Returns true if there is running animations.
  static bool HasAnimationsRunning();

  static void StartAnimation(
      Animation* obj, AnimationCurve animation_curve = Animation::kDefaultCurve,
      uint64_t animation_duration = Animation::kDefaultDuration,
      AnimationTime animation_time = AnimationTime::kFirstUpdate);

  // Aborts the given animation.
  // If delete_animation is true, the animation will be deleted in this call
  // after running callbacks and listeners callbacks. In rare situations, you
  // might want to keep the animation around and delete it later (or start it
  // again).
  static void AbortAnimation(Animation* obj, bool delete_animation);

  // Aborts and deletes all animations.
  static void AbortAllAnimations();

  // Returns true if new animations are blocked.
  static bool IsAnimationsBlocked();

  // Begins a period of blocking new animations. End the period with
  // EndBlockAnimations.
  // If StartAnimation is called during the blocked period, the animation object
  // will finish the next animation update as it completed normally.
  static void BeginBlockAnimations();

  // Ends a period of blocking new animations that was started with
  // BeginBlockAnimations.
  static void EndBlockAnimations();
};

// Blocks new animations during its lifetime.
// It's convenient to put on the stack to block new animations within a scope of
// code.
class AnimationBlocker {
 public:
  AnimationBlocker() { AnimationManager::BeginBlockAnimations(); }
  ~AnimationBlocker() { AnimationManager::EndBlockAnimations(); }
};

// An animated float value.
class FloatAnimation : public Animation {
 public:
  float src_val;
  float dst_val;
  float current_progress = 0;

 public:
  TBOBJECT_SUBCLASS(FloatAnimation, Animation);

  FloatAnimation(float initial_value,
                 AnimationCurve animation_curve = Animation::kDefaultCurve,
                 uint64_t animation_duration = Animation::kDefaultDuration)
      : src_val(initial_value), dst_val(initial_value) {
    Animation::animation_curve = animation_curve;
    Animation::animation_duration = animation_duration;
  }

  float GetValue() { return src_val + (dst_val - src_val) * current_progress; }
  void SetValueAnimated(float value) {
    src_val = GetValue();
    dst_val = value;
    AnimationManager::StartAnimation(this, animation_curve, animation_duration);
  }
  void SetValueImmediately(float value) {
    AnimationManager::AbortAnimation(this, false);
    src_val = dst_val = value;
    OnAnimationUpdate(1.0f);
  }

  void OnAnimationStart() override { current_progress = 0; }
  void OnAnimationUpdate(float progress) override {
    current_progress = progress;
  }
  void OnAnimationStop(bool aborted) override {}
};

// Animates a external float value, which address is given in the constructor.
class ExternalFloatAnimation : public FloatAnimation {
 public:
  float* target_value;

 public:
  TBOBJECT_SUBCLASS(ExternalFloatAnimation, Animation);

  ExternalFloatAnimation(
      float* target_value,
      AnimationCurve animation_curve = Animation::kDefaultCurve,
      uint64_t animation_duration = Animation::kDefaultDuration)
      : FloatAnimation(*target_value, animation_curve, animation_duration),
        target_value(target_value) {}

  void OnAnimationStart() override {
    FloatAnimation::OnAnimationStart();
    *target_value = GetValue();
  }
  void OnAnimationUpdate(float progress) override {
    FloatAnimation::OnAnimationUpdate(progress);
    *target_value = GetValue();
  }
};

}  // namespace tb

#endif  // TB_ANIMATION_H_