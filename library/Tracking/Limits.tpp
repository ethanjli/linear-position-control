#ifndef Tracking_Limits_tpp
#define Tracking_Limits_tpp

namespace LinearPositionControl { namespace Tracking {

// AbsoluteLimits

template <class Limits>
AbsoluteLimits<Limits>::AbsoluteLimits(
    Components::Motor &motor,
    Limits &limits
) :
  motor(motor), limits(limits) {
}

template <class Limits>
void AbsoluteLimits<Limits>::setup() {
  if (setupCompleted) return;

  motor.setup();
  limits.setup();

  state.setup(State::unknown);

  setupCompleted = true;
}

template <class Limits>
void AbsoluteLimits<Limits>::update() {
  limits.update();

  switch (limits.state.current()) {
    case State::left:
    case State::right:
      lastLimit.update(limits.state.current());
    case State::both:
    case State::none:
    case State::unknown:
      state.update(limits.state.current());
      return;
  }
  // limits.state.current() == State::either
  if (limits.state.previous() == State::none) limitSwitchPressDirection = motor.resumeDirection();
  if (limitSwitchPressDirection == FORWARD) {
    state.update(State::right);
    lastLimit.update(State::right, true);
  } else { // limitSwitchPressDirection == BACKWARD
    state.update(State::left);
    lastLimit.update(State::left, true);
  }
}

} }

#endif

