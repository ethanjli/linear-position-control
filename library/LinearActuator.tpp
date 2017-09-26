#ifndef LinearActuator_tpp
#define LinearActuator_tpp

// AbsoluteLinearActuator

template <bool debug_serial>
AbsoluteLinearActuator<debug_serial>::AbsoluteLinearActuator(Motor &motor, AbsoluteLimits &limits) :
  motor(motor), limits(limits) {}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::setup() {
  if (setupCompleted) return;

  motor.setup();
  limits.setup();

  directionCalibrationState = States::DirectionCalibration::uncalibrated;

  setupCompleted = true;
}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::update() {
  using States::DirectionCalibration;

  limits.update();
  switch (directionCalibrationState) {
    case DirectionCalibration::calibrated:
      updateDirectionCalibrated();
      break;
    case DirectionCalibration::uncalibrated:
      updateDirectionUncalibrated();
      break;
    case DirectionCalibration::calibrating:
      updateDirectionCalibrating();
      break;
  }
}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::updateDirectionUncalibrated() {
  using States::Limits;
  using States::DirectionCalibration;

  if (limits.state == Limits::both) return; // delay calibration as long as both limit switches are pressed

  // Start calibration
  directionCalibrationState = DirectionCalibration::calibrating;
  if (debug_serial) Serial.println("Calibrating...");
  motorStallTimer = 0;
  motor.forwards();
}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::updateDirectionCalibrating() {
  using States::Motor;
  using States::Limits;
  using States::DirectionCalibration;

  if (limits.state == Limits::both) { // cancel calibration if both limit switches are pressed
    directionCalibrationState = DirectionCalibration::uncalibrated;
    if (debug_serial) Serial.println("Restarting calibration...");
    return;
  }
  if (limits.state == Limits::none) { // let the motor continue to run until it hits a limit
    return;
  }
  if (limits.previousState == Limits::none) { // the motor just hit a limit switch
    if (debug_serial) Serial.println("Just hit a limit switch!");
    if ((motor.state == Motor::forwards && limits.state == Limits::left) ||
        (motor.state == Motor::backwards && limits.state == Limits::right)) {
      if (debug_serial) Serial.println("Flipping directions!");
      motor.swapDirections();
    }
    motor.forwards();
    directionCalibrationState = DirectionCalibration::calibrated;
    if (debug_serial) {
      Serial.println("Calibrated!");
      if (motor.directionsSwapped()) Serial.print("We flipped the motor's pins in software.");
      else Serial.print("We don't need to flip the motor's pins.");
    }
    digitalWrite(LED_BUILTIN, HIGH);
    return;
  }
  if (motorStallTimer > motorStallTimeout) {
    if (debug_serial) Serial.println("Stall detected!");
    if (motor.state == Motor::forwards) {
      if (debug_serial) Serial.println("Running the motor backwards to break the stall!");
      motor.backwards();
    } else { // motor.state == Motor::backwards
      if (debug_serial) Serial.println("Running the motor forwards to break the stall!");
      motor.forwards();
    }
    motorStallTimer = 0;
  }
}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::updateDirectionCalibrated() {
  using States::PositionCalibration;

  switch (positionCalibrationState) {
    case PositionCalibration::calibrated:
      updatePositionCalibrated();
      break;
    case PositionCalibration::uncalibrated:
      updatePositionUncalibrated();
      break;
    case PositionCalibration::calibrating:
      updatePositionCalibrating();
      break;
  }
}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::updatePositionCalibrated() {
}
template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::updatePositionUncalibrated() {
}
template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::updatePositionCalibrating() {
}

#endif

