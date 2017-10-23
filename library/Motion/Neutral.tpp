#ifndef Motion_Neutral_tpp
#define Motion_Neutral_tpp

#include <ArduinoLog.h>

namespace LinearPositionControl { namespace Motion {

// Neutral

template <class PositionTracker>
Neutral<PositionTracker>::Neutral(Components::Motor &motor, PositionTracker &positionTracker) :
  motor(motor) {
}

template <class PositionTracker>
void Neutral<PositionTracker>::setup() {
  if (setupCompleted) return;

  motor.setup();

  setupCompleted = true;
}

template <class PositionTracker>
void Neutral<PositionTracker>::update() {
  if (motor.state.current() != Components::Motor::State::neutral) {
    Log.notice(F("Setting motor to neutral!" CR));
    motor.neutral();
  }
}

} }

#endif

