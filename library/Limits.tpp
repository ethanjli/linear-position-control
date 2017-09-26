#ifndef Limits_tpp
#define Limits_tpp

// AbsoluteLimits
template <bool debug_serial>
AbsoluteLimits<debug_serial>::AbsoluteLimits(DebouncedButton *leftLimit, DebouncedButton *rightLimit) :
  leftLimit(leftLimit), rightLimit(rightLimit) {}

template <bool debug_serial>
void AbsoluteLimits<debug_serial>::setup() {
  leftLimit->setup();
  rightLimit->setup();

  update();
  previousState = state;
}

template <bool debug_serial>
void AbsoluteLimits<debug_serial>::update() {
  using States::Limits;

  previousState = state;

  leftLimit->update();
  rightLimit->update();

  if (leftLimit->isPressed() && rightLimit->isPressed()) {
    state = Limits::both;
  } else if (leftLimit->isPressed()) {
    state = Limits::left;
  } else if (rightLimit->isPressed()) {
    state = Limits::right;
  } else {
    state = Limits::none;
  }
}

#endif

