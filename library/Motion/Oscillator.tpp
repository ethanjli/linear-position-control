#ifndef Motion_Oscillator_tpp
#define Motion_Oscillator_tpp

#include <ArduinoLog.h>

namespace LinearPositionControl { namespace Motion {

// Oscillator

template <class Limits>
Oscillator<Limits>::Oscillator(Components::Motor &motor, Limits &limits) :
  motor(motor), limits(limits) {
}

template <class Limits>
void Oscillator<Limits>::setup() {
  if (setupCompleted) return;

  motor.setup();
  limits.setup();

  state.setup(State::initializing);

  setupCompleted = true;
}

template <class Limits>
void Oscillator<Limits>::update() {
  limits.update();
  switch (state.current()) {
    case State::operating:
      updateOperating();
      break;
    case State::initializing:
      updateInitializing();
      break;
  }
}

template <class Limits>
void Oscillator<Limits>::updateInitializing() {
  Log.notice(F("Oscillating the motor between the limits!" CR));
  motor.opposite(); // we assume that the actuator is either floating or at the left limit, which is true after direction calibration
  state.update(State::operating);
}

template <class Limits>
void Oscillator<Limits>::updateOperating() {
  if (limits.state.current() == Limits::State::none || !limits.state.justChanged()) {
    // Nothing to do
    return;
  }
  // TODO: we should just use a position tracker to repeatedly go to positions 0 and 1...
  switch (limits.state.current()) {
    case Limits::State::left:
      Log.trace(F("Hit left limit switch. Moving forwards!" CR));
      motor.forwards();
      break;
    case Limits::State::right:
      Log.trace(F("Hit right limit switch. Moving backwards!" CR));
      motor.backwards();
      break;
    case Limits::State::either:
      Log.trace(F("Hit one of the limit switches. Moving in the opposite direction!" CR));
      motor.opposite(); // FIXME: this behavior is not robust! We need position tracking to tell us which limit we're at.
      break;
    case Limits::State::both:
      Log.trace(F("Both limit switches are pressed. Braking!" CR));
      motor.brake(); // FIXME: we actually need to go back to floating
      break;
  }
}

} }

#endif

