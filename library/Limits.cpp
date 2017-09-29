#include "Limits.h"

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
  using States::Limits;

  previousState = state;

  leftLimit.update();
  rightLimit.update();

  if (leftLimit.isPressed() && rightLimit.isPressed()) {
    state = Limits::both;
  } else if (leftLimit.isPressed()) {
    state = Limits::left;
  } else if (rightLimit.isPressed()) {
    state = Limits::right;
  } else {
    state = Limits::none;
  }
}

