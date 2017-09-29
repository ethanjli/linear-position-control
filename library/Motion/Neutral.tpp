#ifndef Neutral_tpp
#define Neutral_tpp

// Neutral

template <bool debug_serial>
Neutral<debug_serial>::Neutral(Motor &motor) :
  motor(motor) {
}

template <bool debug_serial>
void Neutral<debug_serial>::setup() {
  if (setupCompleted) return;

  motor.setup();

  setupCompleted = true;
}

template <bool debug_serial>
void Neutral<debug_serial>::update() {
  if (motor.state != States::Motor::neutral) {
    if (debug_serial) Serial.println(F("Setting motor to neutral!"));
    motor.neutral();
  }
}

#endif

