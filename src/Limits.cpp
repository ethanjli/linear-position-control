#include "Limits.h"

namespace LinearPositionControl { namespace Components {

// AbsoluteLimits

AbsoluteLimits::AbsoluteLimits(DebouncedButton &leftLimit, DebouncedButton &rightLimit) :
  leftLimit(leftLimit), rightLimit(rightLimit) {
}

void AbsoluteLimits::setup() {
  if (setupCompleted) return;

  leftLimit.setup();
  rightLimit.setup();
  state.setup(State::unknown);

  setupCompleted = true;
}

void AbsoluteLimits::update() {
  leftLimit.update();
  rightLimit.update();
  state.update(currentState());
}

AbsoluteLimits::State AbsoluteLimits::currentState() {
  if (leftLimit.state.current() == DebouncedButton::State::pressed &&
      rightLimit.state.current() == DebouncedButton::State::pressed) {
    return State::both;
  } else if (leftLimit.state.current() == DebouncedButton::State::bouncing ||
      rightLimit.state.current() == DebouncedButton::State::bouncing) {
    // Wait for bouncing to stop before updating state
    return state.current();
  } else if (leftLimit.state.current() == DebouncedButton::State::pressed) {
    return State::left;
  } else if (rightLimit.state.current() == DebouncedButton::State::pressed) {
    return State::right;
  } else {
    return State::none;
  }
}

// MultiplexedLimits

MultiplexedLimits::MultiplexedLimits(DebouncedButton &leftAndRightLimits) :
  leftAndRightLimits(leftAndRightLimits) {
}

void MultiplexedLimits::setup() {
  if (setupCompleted) return;

  leftAndRightLimits.setup();
  state.setup(State::unknown);

  setupCompleted = true;
}

void MultiplexedLimits::update() {
  leftAndRightLimits.update();
  state.update(currentState());
}

MultiplexedLimits::State MultiplexedLimits::currentState() {
  if (leftAndRightLimits.state.current() == DebouncedButton::State::pressed) {
    return State::either;
  } else if (leftAndRightLimits.state.current() == DebouncedButton::State::bouncing) {
    return state.current();
  } else {
    return State::none;
  }
}

} }

