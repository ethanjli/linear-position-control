#ifndef Motion_Oscillator_tpp
#define Motion_Oscillator_tpp

#include <ArduinoLog.h>

namespace LinearPositionControl { namespace Motion {

// Oscillator

template <class PositionTracker>
Oscillator<PositionTracker>::Oscillator(Components::Motor &motor, PositionTracker &positionTracker) :
  motor(motor), positionTracker(positionTracker) {
}

template <class PositionTracker>
void Oscillator<PositionTracker>::setup() {
  if (setupCompleted) return;

  motor.setup();
  positionTracker.setup();

  state.setup(State::initializing);

  setupCompleted = true;
}

template <class PositionTracker>
void Oscillator<PositionTracker>::update() {
  positionTracker.update();
  switch (state.current()) {
    case State::operating:
      updateOperating();
      break;
    case State::initializing:
      updateInitializing();
      break;
  }
}

template <class PositionTracker>
void Oscillator<PositionTracker>::updateInitializing() {
  Log.notice(F("Oscillating the motor between the limits!" CR));
  motor.opposite(); // we assume that the actuator is either floating or at the left limit, which is true after direction calibration
  state.update(State::operating);
}

template <class PositionTracker>
void Oscillator<PositionTracker>::updateOperating() {
  if (!positionTracker.position.justChanged()) return;
  if (positionTracker.atLeftLimit()) {
      Log.trace(F("Hit left limit switch. Moving forwards!" CR));
      motor.forwards();
  } else if (positionTracker.atRightLimit()) {
      Log.trace(F("Hit right limit switch. Moving backwards!" CR));
      motor.backwards();
  }
}

} }

#endif

