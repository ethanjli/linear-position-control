#ifndef DirectionCalibration_tpp
#define DirectionCalibration_tpp

#include <ArduinoLog.h>

// DirectionCalibrator

template <class Limits>
DirectionCalibrator<Limits>::DirectionCalibrator(Motor &motor, Limits &limits) :
  motor(motor), limits(limits) {
}

template <class Limits>
void DirectionCalibrator<Limits>::setup() {
  if (setupCompleted) return;

  motor.setup();
  limits.setup();

  state = States::DirectionCalibration::uncalibrated;

  setupCompleted = true;
}

template <class Limits>
void DirectionCalibrator<Limits>::update() {
  using States::DirectionCalibration;

  limits.update();
  switch (state) {
    case DirectionCalibration::uncalibrated:
      updateUncalibrated();
      break;
    case DirectionCalibration::calibrating:
      updateCalibrating();
      break;
    case DirectionCalibration::calibrated: // Nothing to do!
      break;
  }
}


template <class Limits>
void DirectionCalibrator<Limits>::onDirectionCalibrated() {
  using States::DirectionCalibration;

  state = DirectionCalibration::calibrated;
  motor.neutral();

  Log.notice(F("Calibrated!" CR));
  if (motor.directionsSwapped()) Log.notice(F("We flipped the motor's pins in software." CR));
  else Log.notice(F("We don't need to flip the motor's pins." CR));

  digitalWrite(LED_BUILTIN, HIGH);
}


// DirectionCalibrator for AbsoluteLimits

template<>
void DirectionCalibrator<AbsoluteLimits>::updateUncalibrated() {
  using States::Limits;
  using States::DirectionCalibration;

  if (limits.state == Limits::both) return; // delay calibration as long as both limit switches are pressed

  // Start calibration
  state = DirectionCalibration::calibrating;
  Log.notice(F("Calibrating motor direction..." CR));
  motorStallTimer = 0;
  motor.forwards();
}

template<>
void DirectionCalibrator<AbsoluteLimits>::updateMotorDirection() {
  using States::Motor;
  using States::Limits;

  if ((motor.state == Motor::forwards && limits.state == Limits::left) ||
      (motor.state == Motor::backwards && limits.state == Limits::right)) {
    Log.trace(F("Flipping motor directions!" CR));
    motor.swapDirections();
  }
}


template<>
void DirectionCalibrator<AbsoluteLimits>::updateCalibrating() {
  using States::Limits;
  using States::DirectionCalibration;

  if (limits.state == Limits::both) { // cancel calibration if both limit switches are pressed
    state = DirectionCalibration::uncalibrated;
    Log.warning(F("Restarting motor direction calibration..." CR));
    return;
  }
  if (limits.state == Limits::none) return; // let the motor continue to run until it hits a limit
  if (limits.previousState == Limits::none) { // the motor just hit a limit switch
    if (limits.state == Limits::left) Log.trace(F("Just hit the left limit switch!" CR));
    else Log.trace(F("Just hit the right limit switch!" CR)); // limits.state == Limits::right

    updateMotorDirection();
    onDirectionCalibrated();
    return;
  }
  if (motorStallTimer > motorStallTimeout) {
    Log.trace(F("Motor stall detected! Running the motor in the opposite direction..." CR));
    motor.opposite();
    motorStallTimer = 0;
  }
}

// DirectionCalibrator for MultiplexedLimits

template<>
void DirectionCalibrator<MultiplexedLimits>::updateUncalibrated() {
  using States::DirectionCalibration;

  // Start calibration
  state = DirectionCalibration::calibrating;
  Log.notice(F("Calibrating motor direction..." CR));
  motorStallTimer = 0;
  motor.forwards();
}

template<>
void DirectionCalibrator<MultiplexedLimits>::updateCalibrating() {
  using States::Limits;
  using States::DirectionCalibration;

  if (limits.state == Limits::none) { // the motor is now free to move in either direction, which means its direction is "calibrated"
    onDirectionCalibrated();
    return;
  }

  if (motorStallTimer > motorStallTimeout) {
    Log.trace(F("Motor stall detected! Running the motor in the opposite direction..." CR));
    motor.opposite();
    motorStallTimer = 0;
  }
}

#endif

