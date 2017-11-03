#ifndef Calibration_Position_tpp
#define Calibration_Position_tpp

#include <ArduinoLog.h>

namespace LinearPositionControl { namespace Calibration {

// Position

template <class Limits, class EdgeCounter, class PositionTracker>
Position<Limits, EdgeCounter, PositionTracker>::Position(
    Components::Motor &motor,
    Tracking::AbsoluteLimits<Limits> &limitsTracker,
    EdgeCounter &edgeCounter,
    PositionTracker &positionTracker
) :
  motor(motor),
  limitsTracker(limitsTracker),
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

  if (limitsTracker.state.current() == Limits::both ||
      limitsTracker.state.current() == Limits::unknown) {
    return;
  }

  state.update(State::initializing);
  Log.notice(F("Initializing motor position..." CR));
  motor.backwards(255);
}

template<class Limits, class EdgeCounter, class PositionTracker>
void Position<Limits, EdgeCounter, PositionTracker>::updateInitializing() {
  using Components::States::Limits;

  if (limitsTracker.state.current() == Limits::both) {
    state.update(State::uncalibrated);
    Log.warning(F("Restarting motor position calibration..." CR));
    return;
  }

  // Wait for the motor to hit and stabilize at the left limit
  if (limitsTracker.state.current() == Limits::none ||
      limitsTracker.state.current() == Limits::right) {
    return;
  }
  // limitsTracker.state.current() == Limits::left
  if (limitsTracker.state.previous() == Limits::none) {
    motor.brake();
    return;
  }
  if (limitsTracker.state.currentDuration() < limitSwitchTimeout) return;

  // Start calibration
  state.update(State::calibrating);
  if (expectedNumEdges != -1) Log.notice(F("Calibrating motor position, expecting to count %d edges..." CR), expectedNumEdges);
  else Log.notice(F("Calibrating motor position..." CR));
  edgeCounter.reset();
  motor.forwards(calibrationSpeed);
}

template<class Limits, class EdgeCounter, class PositionTracker>
void Position<Limits, EdgeCounter, PositionTracker>::updateCalibrating() {
  using Components::States::Limits;

  if (limitsTracker.state.current() == Limits::both ||
      (limitsTracker.state.current() == Limits::left &&
       state.currentDuration() > limitSwitchTimeout)) {
    // Cancel calibration if both limit switches are pressed or the wrong limit switch is pressed
    motor.neutral();
    state.update(State::uncalibrated);
    Log.warning(F("Restarting motor position calibration..." CR));
    return;
  }

  // Let the motor continue to run until it hits and settles at the right limit
  if (limitsTracker.state.current() != Limits::right) return;
  if (limitsTracker.state.previous() == Limits::none) {
    Log.trace(F("Just hit the right limit switch!" CR));
    motor.brake();
  }
  if (limitsTracker.state.currentDuration() < limitSwitchTimeout) return;

  numEdges = edgeCounter.getAndReset();
  if (expectedNumEdges != -1 && numEdges != expectedNumEdges) {
    Log.error(F("Expected %d edges, counted %d edges. Restarting motor position calibration..." CR), expectedNumEdges, numEdges);
    motor.neutral();
    state.update(State::uncalibrated);
    return;
  }

  // Finish calibration
  positionTracker.updateNumTotalEdges(numEdges);
  onPositionCalibrated();
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

