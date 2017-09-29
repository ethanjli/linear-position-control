#ifndef Motion_Neutral_tpp
#define Motion_Neutral_tpp

#include <ArduinoLog.h>

// Neutral

template <class Limits>
Neutral<Limits>::Neutral(Motor &motor) :
  motor(motor) {
}

template <class Limits>
void Neutral<Limits>::setup() {
  if (setupCompleted) return;

  motor.setup();

  setupCompleted = true;
}

template <class Limits>
void Neutral<Limits>::update() {
  if (motor.state != States::Motor::neutral) {
    Log.notice(F("Setting motor to neutral!" CR));
    motor.neutral();
  }
}

#endif

