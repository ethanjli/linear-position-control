#ifndef LinearActuator_tpp
#define LinearActuator_tpp

// LinearActuator

template <class DirectionCalibrator>
LinearActuator<DirectionCalibrator>::LinearActuator(
    DirectionCalibrator &directionCalibrator
) :
  directionCalibrator(directionCalibrator) {
}

template <class DirectionCalibrator>
void LinearActuator<DirectionCalibrator>::setup() {
  if (setupCompleted) return;

  directionCalibrator.setup();

  setupCompleted = true;
}

template <class DirectionCalibrator>
void LinearActuator<DirectionCalibrator>::update() {
  using States::DirectionCalibration;
  using States::LinearActuator;

  if (directionCalibrator.state != DirectionCalibration::calibrated) {
    state = LinearActuator::calibratingDirection;
    directionCalibrator.update();
    return;
  }
}

#endif

