#ifndef Oscillator_tpp
#define Oscillator_tpp

// Oscillator

template <class Limits, bool debug_serial>
Oscillator<Limits, debug_serial>::Oscillator(Motor &motor, Limits &limits) :
  motor(motor), limits(limits) {
}

template <class Limits, bool debug_serial>
void Oscillator<Limits, debug_serial>::setup() {
  if (setupCompleted) return;

  motor.setup();
  limits.setup();

  state = States::Motion::Oscillator::initializing;

  setupCompleted = true;
}

template <class Limits, bool debug_serial>
void Oscillator<Limits, debug_serial>::update() {
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

template <class Limits, bool debug_serial>
void Oscillator<Limits, debug_serial>::updateInitializing() {
  motor.forwards(); // we assume that the actuator is either floating or at the left limit, which is true after direction calibration
  if (limits.state == States::Limits::none) state = States::Motion::Oscillator::operating;
}

template <class Limits, bool debug_serial>
void Oscillator<Limits, debug_serial>::updateOperating() {
  if (limits.state == States::Limits::none || limits.state == limits.previousState) return; // nothing to do
  // TODO: we should just use a position tracker to repeatedly go to positions 0 and 1...
  switch (limits.state) {
    case States::Limits::left:
      if (debug_serial) Serial.println(F("Hit left limit switch. Moving forwards!"));
      motor.forwards();
      break;
    case States::Limits::right:
      if (debug_serial) Serial.println(F("Hit right limit switch. Moving backwards!"));
      motor.backwards();
      break;
    case States::Limits::either:
      if (debug_serial) Serial.println(F("Hit one of the limit switches. Moving in the opposite direction!"));
      motor.opposite(); // FIXME: this behavior is not robust! We need position tracking to tell us which limit we're at.
      break;
    case States::Limits::both:
      if (debug_serial) Serial.println(F("Both limit switches are pressed. Braking!"));
      motor.brake(); // FIXME: we actually need to go back to floating
      break;
  }
}

#endif

