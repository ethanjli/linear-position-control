#ifndef LinearActuator_tpp
#define LinearActuator_tpp

namespace LinearPositionControl {

// LinearActuator

template <
  class DirectionCalibrator,
  class PositionCalibrator,
  class PositionTracker,
  class MotionController
>
LinearActuator<
  DirectionCalibrator,
  PositionCalibrator,
  PositionTracker,
  MotionController
>::LinearActuator(
    DirectionCalibrator &directionCalibrator,
    PositionCalibrator &positionCalibrator,
    PositionTracker &positionTracker,
    MotionController &motionController
) :
  directionCalibrator(directionCalibrator),
  positionCalibrator(positionCalibrator),
  positionTracker(positionTracker),
  motionController(motionController) {
}

template <
  class DirectionCalibrator,
  class PositionCalibrator,
  class PositionTracker,
  class MotionController
>
void LinearActuator<
  DirectionCalibrator,
  PositionCalibrator,
  PositionTracker,
  MotionController
>::setup() {
  if (setupCompleted) return;

  directionCalibrator.setup();
  positionCalibrator.setup();
  positionTracker.setup();
  state.setup(State::readyToCalibrate);

  setupCompleted = true;
}

template <
  class DirectionCalibrator,
  class PositionCalibrator,
  class PositionTracker,
  class MotionController
>
void LinearActuator<
  DirectionCalibrator,
  PositionCalibrator,
  PositionTracker,
  MotionController
>::update() {
  if (directionCalibrator.state.current() != DirectionCalibrator::State::calibrated) {
    state.update(State::calibratingDirection);
    directionCalibrator.update();
    return;
  }
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

