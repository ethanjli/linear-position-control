#ifndef LinearActuator_tpp
#define LinearActuator_tpp

namespace LinearPositionControl {

// LinearActuator

template <
  class DirectionCalibrator,
  class PositionCalibrator,
  class MotionController
>
LinearActuator<DirectionCalibrator, PositionCalibrator, MotionController>::
LinearActuator(
    DirectionCalibrator &directionCalibrator,
    PositionCalibrator &positionCalibrator,
    MotionController &motionController
) :
  directionCalibrator(directionCalibrator),
  positionCalibrator(positionCalibrator),
  motionController(motionController) {
}

template <
  class DirectionCalibrator,
  class PositionCalibrator,
  class MotionController
>
void LinearActuator<DirectionCalibrator, PositionCalibrator, MotionController>::
setup() {
  if (setupCompleted) return;

  directionCalibrator.setup();
  positionCalibrator.setup();
  state.setup(State::readyToCalibrate);

  setupCompleted = true;
}

template <
  class DirectionCalibrator,
  class PositionCalibrator,
  class MotionController
>
void LinearActuator<DirectionCalibrator, PositionCalibrator, MotionController>::
update() {
  using Calibration::States::Direction;
  using Calibration::States::Position;

  if (directionCalibrator.state.current() != Direction::calibrated) {
    state.update(State::calibratingDirection);
    directionCalibrator.update();
    return;
  }
  if (positionCalibrator.state.current() != Position::calibrated) {
    state.update(State::calibratingPosition);
    positionCalibrator.update();
    return;
  }
  state.update(State::operating);
  motionController.update();
}

}

#endif

