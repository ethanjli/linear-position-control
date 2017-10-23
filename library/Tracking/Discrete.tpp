#ifndef Tracking_Discrete_tpp
#define Tracking_Discrete_tpp

#include <ArduinoLog.h>

namespace LinearPositionControl { namespace Tracking {

// Discrete

template <class Limits, class EdgeCounter>
Discrete<Limits, EdgeCounter>::Discrete(
    Components::Motor &motor,
    Limits &limits,
    EdgeCounter &edgeCounter
) :
  motor(motor), limits(limits), edgeCounter(edgeCounter) {
}

template <class Limits, class EdgeCounter>
void Discrete<Limits, EdgeCounter>::setup() {
  if (setupCompleted) return;

  motor.setup();
  limits.setup();
  edgeCounter.setup();

  state.setup(State::uncalibrated);

  setupCompleted = true;
}

template <class Limits, class EdgeCounter>
void Discrete<Limits, EdgeCounter>::update() {
  limits.update();
  edgeCounter.update();
  switch (state.current()) {
    case State::tracking:
      updateTracking();
      break;
    case State::unlocalized:
      updateUnlocalized();
      break;
    case State::localizing:
      updateLocalizing();
      break;
    case State::uncalibrated: // Wait for someone to call updateNumTotalEdges!
      break;
  }
}

template<class Limits, class EdgeCounter>
void Discrete<Limits, EdgeCounter>::updateNumTotalEdges(int numEdges) {
  numTotalEdges = numEdges;
  state.update(State::unlocalized);
}

template<class Limits, class EdgeCounter>
bool Discrete<Limits, EdgeCounter>::atLeftLimit() const {
  return position.current() == 0;
}

template<class Limits, class EdgeCounter>
bool Discrete<Limits, EdgeCounter>::atRightLimit() const {
  return position.current() == numTotalEdges;
}

template<class Limits, class EdgeCounter>
void Discrete<Limits, EdgeCounter>::updateUnlocalized() {
  using Components::States::Limits;

  if (limits.state.current() == Limits::both ||
      limits.state.current() == Limits::unknown) {
    // Delay localization as long as the limits are unknown or impossible
    return;
  }

  // Start localization
  state.update(State::localizing);
  Log.notice(F("Resetting motor position to nearest limit switch..." CR));
  limitSwitchTimer = 0;
  if (position.current() < numTotalEdges / 2) {
    motor.backwards();
  } else {
    motor.forwards();
  }
}

template<class Limits, class EdgeCounter>
void Discrete<Limits, EdgeCounter>::updateLocalizing() {
  using Components::States::Limits;

  if (limits.state.current() == Limits::both) {
    // Cancel calibration if both limit switches are pressed
    motor.neutral();
    state.update(State::unlocalized);
    Log.warning(F("Restarting motor position localization..." CR));
    return;
  }
  if (limits.state.current() == Limits::none) return; // wait until we've hit a limit

  if (limits.state.previous() == Limits::none) {
    // Brake for a bit to let the motor stabilize at the limit
    limitSwitchTimer = 0;
    Log.trace(F("Just hit a limit switch!" CR));
    motor.brake();
    return;
  }
  if (limitSwitchTimer < limitSwitchTimeout) return; // Wait until we've settled on the left limit

  // Start tracking
  Log.notice(F("Tracking motor position..." CR));
  edgeCounter.getAndReset();
  updateMotorPosition(true);
  onPositionLocalized();
}

template<class Limits, class EdgeCounter>
void Discrete<Limits, EdgeCounter>::updateTracking() {
  edgeCounter.update();
  updateMotorPosition();
}

template<class Limits, class EdgeCounter>
void Discrete<Limits, EdgeCounter>::updateMotorPosition(bool setup) {
  using Components::States::Motor;
  using Components::States::Limits;

  int motorPosition = inferMotorPosition();
  if (motorPosition == -1) state.update(State::unlocalized);

  if (setup) position.setup(motorPosition);
  else position.update(motorPosition);
}

template<class Limits, class EdgeCounter>
int Discrete<Limits, EdgeCounter>::inferMotorPosition() {
  using Components::States::Motor;
  using Components::States::Limits;

  switch (limits.state.current()) {
    case Limits::none: // update position with counter
      switch (motor.state.current()) {
        case Motor::neutral:
          return -1;
        default:
          if (motor.resumeDirection() == FORWARD) {
            return position.current() + edgeCounter.getAndReset();
          } else { // motor.resumeDirection() == BACKWARD
            return position.current() - edgeCounter.getAndReset();
          }
      }
      break;
    case Limits::left:
      return 0;
    case Limits::right:
      return numTotalEdges;
    case Limits::either:
      if (limits.state.previous() == Limits::none) {
        limitSwitchPressDirection = motor.resumeDirection();
      }
      // limitSwitchPressDirection is same even when motor direction changes to release switch
      if (limitSwitchPressDirection == BACKWARD) return 0;
      else return numTotalEdges; // limitSwitchPressDirection == BACKWARD
  }
  return -1; // invalid state
}

template <class Limits, class EdgeCounter>
void Discrete<Limits, EdgeCounter>::onPositionLocalized() {
  state.update(State::tracking);
  motor.brake();

  Log.notice(F("Localized!" CR));
  Log.notice(F("Current position is %d." CR), position.current());
}

} }

#endif

