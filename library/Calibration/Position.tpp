#ifndef Calibration_Position_tpp
#define Calibration_Position_tpp

#include <ArduinoLog.h>

namespace LinearPositionControl { namespace Calibration {

// Position

template <class Limits, class EdgeCounter, class PositionTracker>
Position<Limits, EdgeCounter, PositionTracker>::Position(
    Components::Motor &motor,
    Limits &limits,
    EdgeCounter &edgeCounter,
    PositionTracker &positionTracker
) :
  motor(motor),
  limits(limits),
  edgeCounter(edgeCounter),
  positionTracker(positionTracker) {
}

template <class Limits, class EdgeCounter, class PositionTracker>
void Position<Limits, EdgeCounter, PositionTracker>::setup() {
  if (setupCompleted) return;

  // The motor and limits were set up before/during direction calibration setup, so we skip that
  edgeCounter.setup();

  state.setup(State::uncalibrated);

  setupCompleted = true;
}

template <class Limits, class EdgeCounter, class PositionTracker>
void Position<Limits, EdgeCounter, PositionTracker>::update() {
  limits.update();
  edgeCounter.update();
  switch (state.current()) {
    case State::uncalibrated:
      updateUncalibrated();
      break;
    case State::initializing:
      updateInitializing();
      break;
    case State::calibrating:
      updateCalibrating();
      break;
    case State::calibrated: // Nothing to do!
      break;
  }
}

template<class Limits, class EdgeCounter, class PositionTracker>
void Position<Limits, EdgeCounter, PositionTracker>::updateUncalibrated() {
  using Components::States::Limits;

  if (limits.state.current() == Limits::both ||
      limits.state.current() == Limits::unknown) {
    // Delay calibration as long as the limits are unknown or impossible
    return;
  }

  // Start initialization
  state.update(State::initializing);
  Log.notice(F("Initializing motor position..." CR));
  limitSwitchTimer = 0;
  motor.backwards(255);
}

template<class Limits, class EdgeCounter, class PositionTracker>
void Position<Limits, EdgeCounter, PositionTracker>::updateInitializing() {
  using Components::States::Limits;

  if (limits.state.current() == Limits::both) {
    // Cancel calibration if both limit switches are pressed
    state.update(State::uncalibrated);
    Log.warning(F("Restarting motor position calibration..." CR));
    return;
  }
  if (limits.state.current() == Limits::none ||
      limits.state.current() == Limits::right) {
    // Wait until we've hit the left limit
    return;
  }

  if (limits.state.previous() == Limits::none) {
    // Brake  for a bit to let the motor stabilize at the limit
    limitSwitchTimer = 0;
    motor.brake();
    return;
  }
  if (limitSwitchTimer < limitSwitchTimeout) return; // Wait until we've settled on the left limit

  // Start calibration
  state.update(State::calibrating);
  Log.notice(F("Calibrating motor position..." CR));
  edgeCounter.reset();
  limitSwitchTimer = 0;
  motor.forwards(calibrationSpeed);
}

template<class Limits, class EdgeCounter, class PositionTracker>
void Position<Limits, EdgeCounter, PositionTracker>::updateCalibrating() {
  using Components::States::Limits;

  if (limits.state.current() == Limits::both ||
      (limits.state.current() == Limits::left && limitSwitchTimer > limitSwitchTimeout)) {
    // Cancel calibration if both limit switches are pressed or the wrong limit switch is pressed
    motor.neutral();
    state.update(State::uncalibrated);
    Log.warning(F("Restarting motor position calibration..." CR));
    return;
  }

  if (limits.state.current() == Limits::none) return; // let the motor continue to run until it hits the right limit
  if (limits.state.previous() == Limits::none) { // the motor just hit a limit switch
    Log.trace(F("Just hit the right limit switch!" CR));
    motor.brake();

    if (limitSwitchTimer < limitSwitchTimeout) return; // Wait until we've settled on the right limit
    numEdges = edgeCounter.getAndReset();
    positionTracker.updateNumTotalEdges(numEdges);
    onPositionCalibrated();
    return;
  }
}

template <class Limits, class EdgeCounter, class PositionTracker>
void Position<Limits, EdgeCounter, PositionTracker>::onPositionCalibrated() {
  state.update(State::calibrated);
  motor.brake();

  Log.notice(F("Calibrated!" CR));
  Log.notice(F("There are %d edges between the limits." CR), numEdges);
}

} }

#endif

