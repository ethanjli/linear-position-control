#ifndef LinearActuator_tpp
#define LinearActuator_tpp

namespace LinearPositionControl {

// LinearActuator

template <
  class DirectionCalibrator,
  class LimitsTracker,
  class PositionCalibrator,
  class PositionTracker,
  class MotionController
>
LinearActuator<
  DirectionCalibrator,
  LimitsTracker,
  PositionCalibrator,
  PositionTracker,
  MotionController
>::LinearActuator(
    DirectionCalibrator &directionCalibrator,
    LimitsTracker &limitsTracker,
    PositionCalibrator &positionCalibrator,
    PositionTracker &positionTracker,
    MotionController &motionController
) :
  directionCalibrator(directionCalibrator),
  limitsTracker(limitsTracker),
  positionCalibrator(positionCalibrator),
  positionTracker(positionTracker),
  motionController(motionController) {
}

template <
  class DirectionCalibrator,
  class LimitsTracker,
  class PositionCalibrator,
  class PositionTracker,
  class MotionController
>
void LinearActuator<
  DirectionCalibrator,
  LimitsTracker,
  PositionCalibrator,
  PositionTracker,
  MotionController
>::setup() {
  if (setupCompleted) return;

  directionCalibrator.setup();
  limitsTracker.setup();
  positionCalibrator.setup();
  positionTracker.setup();
  state.setup(State::readyToCalibrate);

  setupCompleted = true;
}

template <
  class DirectionCalibrator,
  class LimitsTracker,
  class PositionCalibrator,
  class PositionTracker,
  class MotionController
>
void LinearActuator<
  DirectionCalibrator,
  LimitsTracker,
  PositionCalibrator,
  PositionTracker,
  MotionController
>::update() {
  if (directionCalibrator.state.current() != DirectionCalibrator::State::calibrated) {
    state.update(State::calibratingDirection);
    directionCalibrator.update();
    return;
  }
  limitsTracker.update();
  if (positionCalibrator.state.current() != PositionCalibrator::State::calibrated) {
    state.update(State::calibratingPosition);
    positionCalibrator.update();
    return;
  }
  if (positionTracker.state.current() != PositionTracker::State::tracking) {
    state.update(State::localizingPosition);
    positionTracker.update();
    return;
  }
  state.update(State::operating);
  motionController.update();
}

}

#endif

