#ifndef Oscillator_tpp
#define Oscillator_tpp

// Oscillator

template <bool debug_serial>
Oscillator<debug_serial>::Oscillator(Motor &motor, AbsoluteLimits &limits) :
  motor(motor), limits(limits) {}

template <bool debug_serial>
void Oscillator<debug_serial>::setup() {
  if (setupCompleted) return;

  motor.setup();
  limits.setup();

  setupCompleted = true;
}

template <bool debug_serial>
void Oscillator<debug_serial>::update() {
  using States::Limits;

  limits.update();
  if (limits.state == limits.previousState) return; // nothing to do
  switch (limits.state) {
    case Limits::left:
      if (debug_serial) Serial.println("Hit left limit switch. Moving forwards!");
      motor.forwards();
      break;
    case Limits::right:
      if (debug_serial) Serial.println("Hit right limit switch. Moving backwards!");
      motor.backwards();
      break;
    case Limits::both:
      if (debug_serial) Serial.println("Both limit switches are pressed. Braking!");
      motor.brake(); // FIXME: we actually need to go back to floating
      break;
    case Limits::either:
      if (debug_serial) Serial.println("Hit one of the limit switches. Moving in the opposite direction!");
      motor.opposite(); // FIXME: this behavior is not robust! We need position tracking to tell us which limit we're at.
      break;
  }
}

#endif

