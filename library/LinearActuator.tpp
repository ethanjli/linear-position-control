#ifndef LinearActuator_tpp
#define LinearActuator_tpp

namespace LinearPositionControl {

// LinearActuator

template <class DirectionCalibrator, class MotionController>
LinearActuator<DirectionCalibrator, MotionController>::LinearActuator(
    DirectionCalibrator &directionCalibrator,
    MotionController &motionController
) :
  directionCalibrator(directionCalibrator),
  motionController(motionController) {
}

template <class DirectionCalibrator, class MotionController>
void LinearActuator<DirectionCalibrator, MotionController>::setup() {
  if (setupCompleted) return;

  directionCalibrator.setup();

  setupCompleted = true;
}

template <class DirectionCalibrator, class MotionController>
void LinearActuator<DirectionCalibrator, MotionController>::update() {
  using Calibration::States::Direction;

  if (directionCalibrator.state != Direction::calibrated) {
    state = States::calibratingDirection;
    directionCalibrator.update();
    return;
  }
  state = States::operating;
  motionController.update();
}

}

#endif

