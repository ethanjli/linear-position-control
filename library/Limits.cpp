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

  update();
  previousState = state;

  setupCompleted = true;
}

void AbsoluteLimits::update() {
  previousState = state;

  leftLimit.update();
  rightLimit.update();

  if (leftLimit.state == DebouncedButton::States::pressed &&
      rightLimit.state == DebouncedButton::States::pressed) {
    state = States::both;
  } else if (leftLimit.state == DebouncedButton::States::pressed) {
    state = States::left;
  } else if (rightLimit.state == DebouncedButton::States::pressed) {
    state = States::right;
  } else {
    state = States::none;
  }
}

// MultiplexedLimits

MultiplexedLimits::MultiplexedLimits(DebouncedButton &leftAndRightLimits) :
  leftAndRightLimits(leftAndRightLimits) {
}

void MultiplexedLimits::setup() {
  if (setupCompleted) return;

  leftAndRightLimits.setup();

  update();
  previousState = state;

  setupCompleted = true;
}

void MultiplexedLimits::update() {
  previousState = state;

  leftAndRightLimits.update();

  if (leftAndRightLimits.state == DebouncedButton::States::pressed) {
    state = States::either;
  } else {
    state = States::none;
  }
}

} }

