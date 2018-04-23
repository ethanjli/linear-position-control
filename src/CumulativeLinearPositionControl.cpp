#include "CumulativeLinearPositionControl.h"

namespace LinearPositionControl {

// CumulativeLinearActuator

CumulativeLinearActuator::CumulativeLinearActuator(
    Components::Motors &motors, MotorPort motorPort,
    uint8_t angleSensorPort, int minPosition, int maxPosition,
    double pidKp, double pidKd, double pidKi, int pidSampleTime,
    bool swapMotorPolarity, int feedforward,
    int brakeLowerThreshold, int brakeUpperThreshold,
    int minDuty, int maxDuty
) :
  motors(motors),
  motor(motors, motorPort),
  angleSensor(angleSensorPort),
  position(angleSensor.state),
  pid(
    position, pidKp, pidKd, pidKi,
    minDuty - feedforward, maxDuty - feedforward, pidSampleTime,
    minPosition, maxPosition
  ),
  speedAdjuster(pid.output, feedforward, brakeLowerThreshold, brakeUpperThreshold),
  swapMotorPolarity(swapMotorPolarity)
{
}

void CumulativeLinearActuator::setup() {
  motors.setup();
  if (swapMotorPolarity) motor.swapDirections();
  angleSensor.setup();
  angleSensor.setZero();
  pid.setup();
}

void CumulativeLinearActuator::update() {
  angleSensor.update();
  pid.update();
  speedAdjuster.update();
  motor.run(speedAdjuster.output.current());
}

void CumulativeLinearActuator::freeze(bool brake) {
  pid.disable();
  speedAdjuster.freeze(brake);
}

void CumulativeLinearActuator::unfreeze() {
  pid.enable();
  speedAdjuster.unfreeze();
}

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
  discretePosition.setup((int) actuator.angleSensor.state.current());

  setupCompleted = true;
}

void CumulativePositionCalibrator::update() {
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
  }
}

bool CumulativePositionCalibrator::calibrated() const {
  return state.current() == State::calibrated;
}

void CumulativePositionCalibrator::updateUncalibrated() {
  actuator.angleSensor.update();
  discretePosition.update((int) actuator.angleSensor.state.current(), true);
  state.update(State::initializing);
  actuator.pid.disable();
  actuator.motor.backwards(calibrationSpeed);
}

void CumulativePositionCalibrator::updateInitializing() {
  actuator.angleSensor.update();
  discretePosition.update((int) actuator.angleSensor.state.current());
  if (discretePosition.currentDuration() < limitTimeout) return;

  actuator.pid.enable();
  actuator.motor.neutral();
  discretePosition.update((int) actuator.angleSensor.state.current(), true);
  state.update(State::calibrating);
}

void CumulativePositionCalibrator::updateCalibrating() {
  actuator.angleSensor.update();
  discretePosition.update((int) actuator.angleSensor.state.current());
  if (discretePosition.currentDuration() < limitTimeout) return;

  actuator.angleSensor.setZero();
  state.update(State::calibrated);
}

// SmoothedCumulativePositionCalibrator

SmoothedCumulativePositionCalibrator::SmoothedCumulativePositionCalibrator(
    CumulativeLinearActuator &actuator,
    ContinuousSmoother &smoother,
    Components::MotorSpeed calibrationSpeed
) :
  actuator(actuator), smoother(smoother), calibrationSpeed(calibrationSpeed)
{}

void SmoothedCumulativePositionCalibrator::setup() {
  if (setupCompleted) return;

  actuator.setup();
  // Set current position to be at the top of smoother's output range
  actuator.angleSensor.setReference(smoother.getMaxInput());
  smoother.setup();

  state.setup(State::uncalibrated);

  setupCompleted = true;
}

void SmoothedCumulativePositionCalibrator::update() {
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
  }
}

bool SmoothedCumulativePositionCalibrator::calibrated() const {
  return state.current() == State::calibrated;
}

void SmoothedCumulativePositionCalibrator::updateUncalibrated() {
  actuator.angleSensor.update();
  smoother.update();
  state.update(State::initializing);
  actuator.freeze(true);
  actuator.motor.backwards(calibrationSpeed);
}

void SmoothedCumulativePositionCalibrator::updateInitializing() {
  actuator.angleSensor.update();
  smoother.update();
  if (smoother.output.currentDuration() < limitTimeout || state.currentDuration() < limitTimeout) return;

  actuator.motor.neutral();
  state.update(State::calibrating);
}

void SmoothedCumulativePositionCalibrator::updateCalibrating() {
  actuator.angleSensor.update();
  smoother.update();
  if (smoother.output.currentDuration() < limitTimeout || state.currentDuration() < limitTimeout) return;

  actuator.angleSensor.setZero();
  state.update(State::calibrated);
}

}

