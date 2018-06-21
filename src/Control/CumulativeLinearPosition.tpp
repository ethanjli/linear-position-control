#ifndef LPC_Control_CumulativeLinearPosition_tpp
#define LPC_Control_CumulativeLinearPosition_tpp

namespace LinearPositionControl { namespace Control {

// CumulativePositionCalibrator

CumulativePositionCalibrator::CumulativePositionCalibrator(
    CumulativeLinearActuator &actuator,
    Components::MotorSpeed calibrationSpeed
) :
  actuator(actuator), calibrationSpeed(calibrationSpeed)
{}

void CumulativePositionCalibrator::setup() {
  if (setupCompleted) return;

  actuator.setup();

  state.setup(State::uncalibrated);
  discretePosition.setup(static_cast<int>(actuator.positionSensor.state.current));

  setupCompleted = true;
}

void CumulativePositionCalibrator::update() {
  switch (state.current) {
    case State::uncalibrated:
      updateUncalibrated();
      break;
    case State::initializing:
      updateInitializing();
      break;
    case State::calibrating:
      updateCalibrating();
      break;
  }
}

bool CumulativePositionCalibrator::calibrated() const {
  return state.at(State::calibrated);
}

void CumulativePositionCalibrator::updateUncalibrated() {
  actuator.positionSensor.update();
  discretePosition.update(static_cast<int>(actuator.positionSensor.state.current), true);
  state.update(State::initializing);
  actuator.pid.disable();
  actuator.motor.backwards(calibrationSpeed);
}

void CumulativePositionCalibrator::updateInitializing() {
  actuator.positionSensor.update();
  discretePosition.update(static_cast<int>(actuator.positionSensor.state.current));
  if (discretePosition.currentDuration() < limitTimeout) return;

  actuator.pid.enable();
  actuator.motor.neutral();
  discretePosition.update(static_cast<int>(actuator.positionSensor.state.current, true));
  state.update(State::calibrating);
}

void CumulativePositionCalibrator::updateCalibrating() {
  actuator.positionSensor.update();
  discretePosition.update(static_cast<int>(actuator.positionSensor.state.current));
  if (discretePosition.currentDuration() < limitTimeout) return;

  actuator.positionSensor.setZero();
  state.update(State::calibrated);
}

// SmoothedCumulativePositionCalibrator

SmoothedCumulativePositionCalibrator::SmoothedCumulativePositionCalibrator(
    CumulativeLinearActuator &actuator,
    Signal::ContinuousSmoother &smoother,
    Components::MotorSpeed calibrationSpeed
) :
  actuator(actuator), smoother(smoother), calibrationSpeed(calibrationSpeed)
{}

void SmoothedCumulativePositionCalibrator::setup() {
  if (setupCompleted) return;

  actuator.setup();
  // Set current position to be at the top of smoother's output range
  actuator.positionSensor.setReference(smoother.getMaxInput());
  smoother.setup();

  state.setup(State::uncalibrated);

  setupCompleted = true;
}

void SmoothedCumulativePositionCalibrator::update() {
  switch (state.current) {
    case State::uncalibrated:
      updateUncalibrated();
      break;
    case State::initializing:
      updateInitializing();
      break;
    case State::calibrating:
      updateCalibrating();
      break;
  }
}

bool SmoothedCumulativePositionCalibrator::calibrated() const {
  return state.at(State::calibrated);
}

void SmoothedCumulativePositionCalibrator::updateUncalibrated() {
  actuator.positionSensor.update();
  smoother.update();
  state.update(State::initializing);
  actuator.freeze(true);
  actuator.motor.backwards(calibrationSpeed);
}

void SmoothedCumulativePositionCalibrator::updateInitializing() {
  actuator.positionSensor.update();
  smoother.update();
  if (smoother.output.currentDuration() < limitTimeout || state.currentDuration() < limitTimeout) return;

  actuator.motor.neutral();
  state.update(State::calibrating);
}

void SmoothedCumulativePositionCalibrator::updateCalibrating() {
  actuator.positionSensor.update();
  smoother.update();
  if (smoother.output.currentDuration() < limitTimeout || state.currentDuration() < limitTimeout) return;

  actuator.positionSensor.setZero();
  state.update(State::calibrated);
}

} }

#endif

