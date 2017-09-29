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

  state = States::initializing;

  setupCompleted = true;
}

template <class Limits>
void Oscillator<Limits>::update() {
  limits.update();
  switch (state) {
    case States::operating:
      updateOperating();
      break;
    case States::initializing:
      updateInitializing();
      break;
  }
}

template <class Limits>
void Oscillator<Limits>::updateInitializing() {
  Log.notice(F("Oscillating the motor between the limits!" CR));
  motor.opposite(); // we assume that the actuator is either floating or at the left limit, which is true after direction calibration
  state = States::operating;
}

template <class Limits>
void Oscillator<Limits>::updateOperating() {
  if (limits.state == Limits::States::none || limits.state == limits.previousState) return; // nothing to do
  // TODO: we should just use a position tracker to repeatedly go to positions 0 and 1...
  switch (limits.state) {
    case Limits::States::left:
      Log.trace(F("Hit left limit switch. Moving forwards!" CR));
      motor.forwards();
      break;
    case Limits::States::right:
      Log.trace(F("Hit right limit switch. Moving backwards!" CR));
      motor.backwards();
      break;
    case Limits::States::either:
      Log.trace(F("Hit one of the limit switches. Moving in the opposite direction!" CR));
      motor.opposite(); // FIXME: this behavior is not robust! We need position tracking to tell us which limit we're at.
      break;
    case Limits::States::both:
      Log.trace(F("Both limit switches are pressed. Braking!" CR));
      motor.brake(); // FIXME: we actually need to go back to floating
      break;
  }
}

} }

#endif

