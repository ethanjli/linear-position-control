#ifndef Motion_Oscillator_tpp
#define Motion_Oscillator_tpp

#include <ArduinoLog.h>

// Oscillator

template <class Limits>
Oscillator<Limits>::Oscillator(Motor &motor, Limits &limits) :
  motor(motor), limits(limits) {
}

template <class Limits>
void Oscillator<Limits>::setup() {
  if (setupCompleted) return;

  motor.setup();
  limits.setup();

  state = States::Motion::Oscillator::initializing;

  setupCompleted = true;
}

template <class Limits>
void Oscillator<Limits>::update() {
  limits.update();
  switch (state) {
    case States::Motion::Oscillator::operating:
      updateOperating();
      break;
    case States::Motion::Oscillator::initializing:
      updateInitializing();
      break;
  }
}

template <class Limits>
void Oscillator<Limits>::updateInitializing() {
  Log.notice(F("Oscillating the motor between the limits!" CR));
  motor.opposite(); // we assume that the actuator is either floating or at the left limit, which is true after direction calibration
  state = States::Motion::Oscillator::operating;
}

template <class Limits>
void Oscillator<Limits>::updateOperating() {
  if (limits.state == States::Limits::none || limits.state == limits.previousState) return; // nothing to do
  // TODO: we should just use a position tracker to repeatedly go to positions 0 and 1...
  switch (limits.state) {
    case States::Limits::left:
      Log.trace(F("Hit left limit switch. Moving forwards!" CR));
      motor.forwards();
      break;
    case States::Limits::right:
      Log.trace(F("Hit right limit switch. Moving backwards!" CR));
      motor.backwards();
      break;
    case States::Limits::either:
      Log.trace(F("Hit one of the limit switches. Moving in the opposite direction!" CR));
      motor.opposite(); // FIXME: this behavior is not robust! We need position tracking to tell us which limit we're at.
      break;
    case States::Limits::both:
      Log.trace(F("Both limit switches are pressed. Braking!" CR));
      motor.brake(); // FIXME: we actually need to go back to floating
      break;
  }
}

#endif

