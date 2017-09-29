#ifndef Calibration_Direction_tpp
#define Calibration_Direction_tpp

#include <ArduinoLog.h>

namespace LinearPositionControl { namespace Calibration {

// Direction

template <class Limits>
Direction<Limits>::Direction(Components::Motor &motor, Limits &limits) :
  motor(motor), limits(limits) {
}

template <class Limits>
void Direction<Limits>::setup() {
  if (setupCompleted) return;

  motor.setup();
  limits.setup();

  state = States::uncalibrated;

  setupCompleted = true;
}

template <class Limits>
void Direction<Limits>::update() {
  limits.update();
  switch (state) {
    case States::uncalibrated:
      updateUncalibrated();
      break;
    case States::calibrating:
      updateCalibrating();
      break;
    case States::calibrated: // Nothing to do!
      break;
  }
}


template <class Limits>
void Direction<Limits>::onDirectionCalibrated() {
  state = States::calibrated;
  motor.neutral();

  Log.notice(F("Calibrated!" CR));
  if (motor.directionsSwapped()) Log.notice(F("We flipped the motor's pins in software." CR));
  else Log.notice(F("We don't need to flip the motor's pins." CR));

  digitalWrite(LED_BUILTIN, HIGH);
}


// Direction for AbsoluteLimits

template<>
void Direction<Components::AbsoluteLimits>::updateUncalibrated() {
  if (limits.state == Components::States::Limits::both) return; // delay calibration as long as both limit switches are pressed

  // Start calibration
  state = States::calibrating;
  Log.notice(F("Calibrating motor direction..." CR));
  motorStallTimer = 0;
  motor.forwards();
}

template<>
void Direction<Components::AbsoluteLimits>::updateMotorDirection() {
  using Components::States::Motor;
  using Components::States::Limits;

  if ((motor.state == Motor::forwards && limits.state == Limits::left) ||
      (motor.state == Motor::backwards && limits.state == Limits::right)) {
    Log.trace(F("Flipping motor directions!" CR));
    motor.swapDirections();
  }
}


template<>
void Direction<Components::AbsoluteLimits>::updateCalibrating() {
  using Components::States::Limits;

  if (limits.state == Limits::both) { // cancel calibration if both limit switches are pressed
    state = States::uncalibrated;
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

// Direction for MultiplexedLimits

template<>
void Direction<Components::MultiplexedLimits>::updateUncalibrated() {
  state = States::calibrating;
  Log.notice(F("Calibrating motor direction..." CR));
  motorStallTimer = 0;
  motor.forwards();
}

template<>
void Direction<Components::MultiplexedLimits>::updateCalibrating() {
  using Components::States::Limits;

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

} }

#endif

