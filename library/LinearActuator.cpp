#include "LinearActuator.h"

namespace States {
  namespace Motor {
    const int BRAKING = 0x00;
    const int FORWARDS = 0x01;
    const int BACKWARDS = 0x02;
  }

  namespace Limits {
    const int NONE = 0x00;
    const int LEFT = 0x01;
    const int RIGHT = 0x02;
    const int EITHER = 0x10;
    const int BOTH = 0x11;
  }

  namespace DirectionCalibration {
    const int UNCALIBRATED = 0x00;
    const int CALIBRATED = 0x01;
    const int CALIBRATING = 0x02;
  }
}

AbsoluteLinearActuator::AbsoluteLinearActuator(Motors *motors, MotorPort motorPort, DebouncedButton *leftLimit, DebouncedButton *rightLimit, bool debug_serial) :
  motors(motors), motorPort(motorPort), leftLimit(leftLimit), rightLimit(rightLimit), debug_serial(debug_serial) {}

void AbsoluteLinearActuator::setup() {
  motors->setup();
  leftLimit->setup();
  rightLimit->setup();

  brake();

  updateLimits();
  previousLimitsState = limitsState;

  directionCalibrationState = States::DirectionCalibration::UNCALIBRATED;
}

void AbsoluteLinearActuator::update() {
  using namespace States::DirectionCalibration;

  updateLimits();
  switch (directionCalibrationState) {
    case UNCALIBRATED:
      updateUncalibrated();
      break;
    case CALIBRATING:
      updateCalibrating();
      break;
    case CALIBRATED:
      updateCalibrated();
      break;
  }
}

void AbsoluteLinearActuator::runForwards() {
  motors->run(motorPort, forwards, speed);
  motorState = States::Motor::FORWARDS;
}

void AbsoluteLinearActuator::runBackwards() {
  motors->run(motorPort, backwards, speed);
  motorState = States::Motor::BACKWARDS;
}

void AbsoluteLinearActuator::brake() {
  motors->brake(motorPort);
  motorState = States::Motor::BRAKING;
}

void AbsoluteLinearActuator::updateLimits() {
  using namespace States::Limits;

  previousLimitsState = limitsState;

  leftLimit->update();
  rightLimit->update();
  if (leftLimit->isPressed() && rightLimit->isPressed()) {
    limitsState = BOTH;
  } else if (leftLimit->isPressed()) {
    limitsState = LEFT;
  } else if (rightLimit->isPressed()) {
    limitsState = RIGHT;
  } else {
    limitsState = NONE;
  }
}

void AbsoluteLinearActuator::updateUncalibrated() {
  using namespace States;

  if (limitsState == Limits::BOTH) return; // delay calibration as long as both limit switches are pressed

  // Start calibration
  directionCalibrationState = DirectionCalibration::CALIBRATING;
  if (debug_serial) Serial.println("Calibrating...");
  motorStallTimer = 0;
  runForwards();
}

void AbsoluteLinearActuator::updateCalibrating() {
  using namespace States;

  if (limitsState == Limits::BOTH) { // cancel calibration if both limit switches are pressed
    motorState = DirectionCalibration::UNCALIBRATED;
    if (debug_serial) Serial.println("Restarting calibration...");
    return;
  }
  if (limitsState == Limits::NONE) { // let the motor continue to run until it hits a limit
    return;
  }
  if (previousLimitsState == Limits::NONE) { // the motor just hit a limit switch
    if (debug_serial) Serial.println("Just hit a limit switch!");
    if ((motorState == Motor::FORWARDS && limitsState == Limits::LEFT) ||
        (motorState == Motor::BACKWARDS && limitsState == Limits::RIGHT)) {
      if (debug_serial) Serial.println("Flipping directions!");
      forwards = BACKWARD;
      backwards = FORWARD;
    }
    runForwards();
    directionCalibrationState = DirectionCalibration::CALIBRATED;
    if (debug_serial) {
      Serial.println("Calibrated!");
      if (forwards == Motor::BACKWARDS) Serial.print("We flipped the motor's pins in software.");
      else Serial.print("We don't need to flip the motor's pins.");
    }
    digitalWrite(LED_BUILTIN, HIGH);
    return;
  }
  if (motorStallTimer > motorStallTimeout) {
    if (debug_serial) Serial.println("Stall detected!");
    if (motorState == Motor::FORWARDS) {
      if (debug_serial) Serial.println("Running the motor backwards to break the stall!");
      runBackwards();
    } else { // motorState == Motor::BACKWARDS
      if (debug_serial) Serial.println("Running the motor forwards to break the stall!");
      runForwards();
    }
    motorStallTimer = 0;
  }
}

void AbsoluteLinearActuator::updateCalibrated() {
  using namespace States;

  if (limitsState == Limits::BOTH) { // recalibrate if both limit switches are pressed
    motorState = DirectionCalibration::UNCALIBRATED;
    return;
  }

  if (limitsState == Limits::LEFT) {
    runForwards();
  } else if (limitsState == Limits::RIGHT) {
    brake();
  }
}

