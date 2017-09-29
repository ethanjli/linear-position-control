#ifndef DirectionCalibration_tpp
#define DirectionCalibration_tpp

// AbsoluteDirectionCalibrator

template <bool debug_serial>
AbsoluteDirectionCalibrator<debug_serial>::AbsoluteDirectionCalibrator(Motor &motor, AbsoluteLimits &limits) :
  motor(motor), limits(limits) {
}

template <bool debug_serial>
void AbsoluteDirectionCalibrator<debug_serial>::setup() {
  if (setupCompleted) return;

  motor.setup();
  limits.setup();

  state = States::DirectionCalibration::uncalibrated;

  setupCompleted = true;
}

template <bool debug_serial>
void AbsoluteDirectionCalibrator<debug_serial>::update() {
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

template <bool debug_serial>
void AbsoluteDirectionCalibrator<debug_serial>::updateUncalibrated() {
  using States::Limits;
  using States::DirectionCalibration;

  if (limits.state == Limits::both) return; // delay calibration as long as both limit switches are pressed

  // Start calibration
  state = DirectionCalibration::calibrating;
  if (debug_serial) Serial.println(F("Calibrating motor direction..."));
  motorStallTimer = 0;
  motor.forwards();
}

template <bool debug_serial>
void AbsoluteDirectionCalibrator<debug_serial>::updateCalibrating() {
  using States::Limits;
  using States::DirectionCalibration;

  if (limits.state == Limits::both) { // cancel calibration if both limit switches are pressed
    state = DirectionCalibration::uncalibrated;
    if (debug_serial) Serial.println(F("Restarting motor direction calibration..."));
    return;
  }
  if (limits.state == Limits::none) return; // let the motor continue to run until it hits a limit
  if (limits.previousState == Limits::none) { // the motor just hit a limit switch
    if (debug_serial) {
      if (limits.state == Limits::left) Serial.println(F("Just hit the left limit switch!"));
      else Serial.println(F("Just hit the right limit switch!")); // limits.state == Limits::right
    }
    updateMotorDirection();
    onDirectionCalibrated();
    return;
  }
  if (motorStallTimer > motorStallTimeout) {
    if (debug_serial) Serial.println(F("Motor stall detected! Running the motor in the opposite direction..."));
    motor.opposite();
    motorStallTimer = 0;
  }
}

template <bool debug_serial>
void AbsoluteDirectionCalibrator<debug_serial>::onDirectionCalibrated() {
  using States::DirectionCalibration;

  state = DirectionCalibration::calibrated;
  motor.neutral();

  if (debug_serial) {
    Serial.println(F("Calibrated!"));
    if (motor.directionsSwapped()) Serial.println(F("We flipped the motor's pins in software."));
    else Serial.println(F("We don't need to flip the motor's pins."));
  }

  digitalWrite(LED_BUILTIN, HIGH);
}

template <bool debug_serial>
void AbsoluteDirectionCalibrator<debug_serial>::updateMotorDirection() {
  using States::Motor;
  using States::Limits;

  if ((motor.state == Motor::forwards && limits.state == Limits::left) ||
      (motor.state == Motor::backwards && limits.state == Limits::right)) {
    if (debug_serial) Serial.println(F("Flipping motor directions!"));
    motor.swapDirections();
  }
}

// MultiplexedDirectionCalibrator

template <bool debug_serial>
MultiplexedDirectionCalibrator<debug_serial>::MultiplexedDirectionCalibrator(Motor &motor, MultiplexedLimits &limits) :
  motor(motor), limits(limits) {
}

template <bool debug_serial>
void MultiplexedDirectionCalibrator<debug_serial>::setup() {
  if (setupCompleted) return;

  motor.setup();
  limits.setup();

  state = States::DirectionCalibration::uncalibrated;

  setupCompleted = true;
}

template <bool debug_serial>
void MultiplexedDirectionCalibrator<debug_serial>::update() {
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

template <bool debug_serial>
void MultiplexedDirectionCalibrator<debug_serial>::updateUncalibrated() {
  using States::DirectionCalibration;

  // Start calibration
  state = DirectionCalibration::calibrating;
  if (debug_serial) Serial.println(F("Calibrating motor direction..."));
  motorStallTimer = 0;
  motor.forwards();
}

template <bool debug_serial>
void MultiplexedDirectionCalibrator<debug_serial>::updateCalibrating() {
  using States::Limits;
  using States::DirectionCalibration;

  if (limits.state == Limits::none) { // the motor is now free to move in either direction, which means its direction is "calibrated"
    onDirectionCalibrated();
    return;
  }

  if (motorStallTimer > motorStallTimeout) {
    if (debug_serial) Serial.println(F("Motor stall detected! Running the motor in the opposite direction..."));
    motor.opposite();
    motorStallTimer = 0;
  }
}

template <bool debug_serial>
void MultiplexedDirectionCalibrator<debug_serial>::onDirectionCalibrated() {
  using States::DirectionCalibration;

  state = DirectionCalibration::calibrated;
  motor.neutral();

  if (debug_serial) Serial.println(F("Calibrated!"));

  digitalWrite(LED_BUILTIN, HIGH);
}

#endif

