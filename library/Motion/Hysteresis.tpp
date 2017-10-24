#ifndef Motion_Hysteresis_tpp
#define Motion_Hysteresis_tpp

#include <ArduinoLog.h>

namespace LinearPositionControl { namespace Motion {

// Hysteresis

template <class PositionTracker>
Hysteresis<PositionTracker>::Hysteresis(Components::Motor &motor, PositionTracker &positionTracker) :
  motor(motor), positionTracker(positionTracker) {
}

template <class PositionTracker>
void Hysteresis<PositionTracker>::setup() {
  if (setupCompleted) return;

  motor.setup();
  positionTracker.setup();

  state.setup(State::targeting);
  targetPosition.setup(-1);

  setupCompleted = true;
}

template <class PositionTracker>
void Hysteresis<PositionTracker>::update() {
  positionTracker.update();
  if (positionTracker.state.current() != PositionTracker::State::tracking) return;

  switch (state.current()) {
    case State::targeting:
      updateTargeting();
      break;
    case State::adjusting:
      updateAdjusting();
      break;
    case State::maintaining:
      updateMaintaining();
      break;
  }
}

template <class PositionTracker>
void Hysteresis<PositionTracker>::updateTargeting() {
  if (targetPosition.justEntered(-1)) {
    Log.notice(F("Ignoring invalid target position %d!" CR), targetPosition.current());
    return;
  }
  Log.notice(F("Targeting position %d!" CR), targetPosition.current());
  targetPosition.update(targetPosition.current()); // confirm that we're targeting to start adjusting
  state.update(State::adjusting);
}

template <class PositionTracker>
void Hysteresis<PositionTracker>::updateAdjusting() {
  if (targetPosition.justChanged()) {
    state.update(State::targeting);
    return;
  }
  if (positionTracker.position.current() < targetPosition.current()) {
    motor.forwards();
  } else if (positionTracker.position.current() > targetPosition.current()) {
    motor.backwards();
  } else { // positionTracker.position.current() == targetPosition
    state.update(State::maintaining);
  }
}

template <class PositionTracker>
void Hysteresis<PositionTracker>::updateMaintaining() {
  if (targetPosition.justChanged()) {
    state.update(State::targeting);
    return;
  }
  if (!state.justEntered(State::maintaining)) return;

  Log.notice(F("Reached target position %d. Braking!" CR), targetPosition.current());
  motor.brake();
  state.update(State::maintaining); // confirm that we're maintaining
}

} }

#endif

