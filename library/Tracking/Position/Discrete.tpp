#ifndef Tracking_Position_Discrete_tpp
#define Tracking_Position_Discrete_tpp

namespace LinearPositionControl { namespace Tracking { namespace Position {

// Discrete

template <class Limits, class EdgeCounter>
Discrete<Limits, EdgeCounter>::Discrete(
    Components::Motor &motor,
    Tracking::AbsoluteLimits<Limits> &limitsTracker,
    EdgeCounter &edgeCounter
) :
  motor(motor), limitsTracker(limitsTracker), edgeCounter(edgeCounter) {
}

template <class Limits, class EdgeCounter>
void Discrete<Limits, EdgeCounter>::setup() {
  if (setupCompleted) return;

  motor.setup();
  limitsTracker.setup();
  edgeCounter.setup();

  state.setup(State::uncalibrated);

  setupCompleted = true;
}

template <class Limits, class EdgeCounter>
void Discrete<Limits, EdgeCounter>::update() {
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
int Discrete<Limits, EdgeCounter>::getNumTotalEdges() const {
  return numTotalEdges;
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
int Discrete<Limits, EdgeCounter>::mapPositionTo(int position, int toMin, int toMax) const {
  return map(position, 0, numTotalEdges, toMin, toMax);
}

template<class Limits, class EdgeCounter>
int Discrete<Limits, EdgeCounter>::mapPositionFrom(int position, int fromMin, int fromMax) const {
  return map(position, fromMin, fromMax, 0, numTotalEdges);
}

template<class Limits, class EdgeCounter>
void Discrete<Limits, EdgeCounter>::updateUnlocalized() {
  using Components::States::Limits;

  if (limitsTracker.state.current() == Limits::both ||
      limitsTracker.state.current() == Limits::unknown) {
    // Delay localization as long as the limits are unknown or impossible
    return;
  }

  // Start localization
  state.update(State::localizing);
  Log.trace(F("Resetting motor position to nearest limit switch..." CR));
  if (position.current() < numTotalEdges / 2) {
    motor.backwards(relocalizationSpeed);
  } else {
    motor.forwards(relocalizationSpeed);
  }
}

template<class Limits, class EdgeCounter>
void Discrete<Limits, EdgeCounter>::updateLocalizing() {
  using Components::States::Limits;

  if (limitsTracker.state.current() == Limits::both) {
    // Cancel calibration if both limit switches are pressed
    motor.neutral();
    state.update(State::unlocalized);
    Log.warning(F("Restarting motor position localization..." CR));
    return;
  }
  if (limitsTracker.state.current() == Limits::none) return; // wait until we've hit a limit

  if (limitsTracker.state.previous() == Limits::none) {
    // Brake for a bit to let the motor stabilize at the limit
    onLimitPressed(limitsTracker.state.current());
    if (limitsTracker.state.current() == Limits::right) {
      Log.trace(F("Just hit the right limit switch!" CR));
    } else { // limitsTracker.state.current() == Limits::left
      Log.trace(F("Just hit the left limit switch!" CR));
    }
    motor.brake();
    return;
  }
  if (limitsTracker.state.currentDuration() < limitSwitchTimeout) return; // Wait until we've settled on the limit

  // Start tracking
  edgeCounter.getAndReset();
  updateMotorPosition(true);
  Log.notice(F("Localized! Current position is %d." CR), position.current());
  state.update(State::tracking);
  motor.brake();
  Log.trace(F("Tracking motor position..." CR));
}

template<class Limits, class EdgeCounter>
void Discrete<Limits, EdgeCounter>::updateTracking() {
  edgeCounter.update();
  updateMotorPosition();
  if (state.current() == State::unlocalized) return; // updateMotorPosition detected an error
  if (position.current() < 0 || position.current() > numTotalEdges) { // invalid position
    Log.error(F("Relocalization required with invalid position %d." CR), position.current());
    state.update(State::unlocalized);
  }
}

template<class Limits, class EdgeCounter>
void Discrete<Limits, EdgeCounter>::updateMotorPosition(bool setup) {
  using Components::States::Motor;
  using Components::States::Limits;

  bool error;
  int motorPosition = inferMotorPosition(error);
  if (error) state.update(State::unlocalized);

  if (setup) {
    position.setup(motorPosition, true);
    Log.trace(F("Finished localization at position %d." CR), motorPosition);
  } else {
    position.update(motorPosition);
  }
}

template<class Limits, class EdgeCounter>
int Discrete<Limits, EdgeCounter>::inferMotorPosition(bool &error) {
  using Components::States::Motor;
  using Components::States::Limits;

  error = false;
  switch (limitsTracker.state.current()) {
    case Limits::left:
      onLimitPressed(Limits::left);
      return 0;
    case Limits::right:
      onLimitPressed(Limits::right);
      return numTotalEdges;
    case Limits::none: // update position with counter
      switch (motor.state.current()) {
        case Motor::neutral:
        //case Motor::braking:
          if (edgeCounter.getAndReset()) {
            Log.warning(F("Relocalization required because of motion while the motor was stopped." CR));
            error = true;
          }
          return position.current();
        default:
          int newPosition;
          uint8_t edgesCounted = edgeCounter.getAndReset();
          if (motor.resumeDirection() == FORWARD) {
            newPosition = position.current() + edgesCounted;
            forwardsEdgesSinceLastLimit += edgesCounted;
            if (newPosition == numTotalEdges) {
              Log.trace(F("Relocalizing to right limit switch..." CR));
              error = true;
            }
          } else { // motor.resumeDirection() == BACKWARD
            newPosition = position.current() - edgesCounted;
            backwardsEdgesSinceLastLimit += edgesCounted;
            if (newPosition == 0) {
              Log.trace(F("Relocalizing to left limit switch..." CR));
              error = true;
            }
          }
          return newPosition;
      }
  }
  Log.warning(F("Relocalization required with invalid limits state." CR));
  error = true;
  return position.current();
}

template<class Limits, class EdgeCounter>
void Discrete<Limits, EdgeCounter>::onLimitPressed(Components::States::Limits state) {
  forwardsEdgesSinceLastLimit = 0;
  backwardsEdgesSinceLastLimit = 0;
}

} } }

#endif
