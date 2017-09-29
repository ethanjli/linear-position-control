#ifndef Motion_Neutral_tpp
#define Motion_Neutral_tpp

#include <ArduinoLog.h>

namespace LinearPositionControl { namespace Motion {

// Neutral

template <class Limits>
Neutral<Limits>::Neutral(Components::Motor &motor) :
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
  if (motor.state != Components::Motor::States::neutral) {
    Log.notice(F("Setting motor to neutral!" CR));
    motor.neutral();
  }
}

} }

#endif

