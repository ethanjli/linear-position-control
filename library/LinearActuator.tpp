#ifndef LinearActuator_tpp
#define LinearActuator_tpp

// AbsoluteLinearActuator

template <class DirectionCalibrator>
AbsoluteLinearActuator<DirectionCalibrator>::AbsoluteLinearActuator(
    DirectionCalibrator &directionCalibrator
) :
  directionCalibrator(directionCalibrator) {
}

template <class DirectionCalibrator>
void AbsoluteLinearActuator<DirectionCalibrator>::setup() {
  if (setupCompleted) return;

  directionCalibrator.setup();

  setupCompleted = true;
}

template <class DirectionCalibrator>
void AbsoluteLinearActuator<DirectionCalibrator>::update() {
  using States::DirectionCalibration;
  using States::LinearActuator;

  if (directionCalibrator.state != DirectionCalibration::calibrated) {
    state = LinearActuator::calibratingDirection;
    directionCalibrator.update();
    return;
  }
}

#endif

