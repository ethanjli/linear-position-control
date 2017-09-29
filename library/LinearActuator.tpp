#ifndef LinearActuator_tpp
#define LinearActuator_tpp

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
  using States::DirectionCalibration;
  using States::LinearActuator;

  if (directionCalibrator.state != DirectionCalibration::calibrated) {
    state = LinearActuator::calibratingDirection;
    directionCalibrator.update();
    return;
  }
  motionController.update();
}

#endif

