#include "CumulativeLinearPositionControl.h"

namespace LinearPositionControl {

// CumulativeLinearActuator

CumulativeLinearActuator::CumulativeLinearActuator(
    Components::Motors &motors, MotorPort motorPort,
    int minPosition, int maxPosition,
    double pidKp, double pidKd, double pidKi, int pidSampleTime,
    bool swapMotorPolarity, int feedforward,
    int brakeLowerThreshold, int brakeUpperThreshold,
    int minDuty, int maxDuty
) :
  motors(motors),
  motor(motors, motorPort),
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

void CumulativeLinearActuator::freeze() {
  pid.disable();
  motor.brake();
}

void CumulativeLinearActuator::unfreeze() {
  pid.enable();
}

// CumulativePositionCalibrator

CumulativePositionCalibrator::CumulativePositionCalibrator(
    CumulativeLinearActuator &actuator
) :
  actuator(actuator)
{}

void CumulativePositionCalibrator::setup() {
  if (setupCompleted) return;

  actuator.setup();

  state.setup(State::uncalibrated);
  discretePosition.setup((long) actuator.angleSensor.state.current());

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

}

