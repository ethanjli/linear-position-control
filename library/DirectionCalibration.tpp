#ifndef DirectionCalibration_tpp
#define DirectionCalibration_tpp

// AbsoluteDirectionCalibrator

template <bool debug_serial>
AbsoluteDirectionCalibrator<debug_serial>::AbsoluteDirectionCalibrator(Motor &motor, AbsoluteLimits &limits) :
  motor(motor), limits(limits) {}

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
  if (debug_serial) Serial.println("Calibrating motor direction...");
  motorStallTimer = 0;
  motor.forwards();
}

template <bool debug_serial>
void AbsoluteDirectionCalibrator<debug_serial>::updateCalibrating() {
  using States::Limits;
  using States::DirectionCalibration;

  if (limits.state == Limits::both) { // cancel calibration if both limit switches are pressed
    state = DirectionCalibration::uncalibrated;
    if (debug_serial) Serial.println("Restarting motor direction calibration...");
    return;
  }
  if (limits.state == Limits::none) return; // let the motor continue to run until it hits a limit
  if (limits.previousState == Limits::none) { // the motor just hit a limit switch
    if (debug_serial) {
      if (limits.state == Limits::left) Serial.println("Just hit the left limit switch!");
      else Serial.println("Just hit the right limit switch!"); // limits.state == Limits::right
    }
    updateMotorDirection();
    onDirectionCalibrated();
    return;
  }
  if (motorStallTimer > motorStallTimeout) {
    if (debug_serial) Serial.println("Stall detected!");
    breakStall();
    motorStallTimer = 0;
  }
}

template <bool debug_serial>
void AbsoluteDirectionCalibrator<debug_serial>::onDirectionCalibrated() {
  using States::DirectionCalibration;

  motor.backwards();
  state = DirectionCalibration::calibrated;

  if (debug_serial) {
    Serial.println("Calibrated!");
    if (motor.directionsSwapped()) Serial.println("We flipped the motor's pins in software.");
    else Serial.println("We don't need to flip the motor's pins.");
  }

  digitalWrite(LED_BUILTIN, HIGH);
}

template <bool debug_serial>
void AbsoluteDirectionCalibrator<debug_serial>::updateMotorDirection() {
  using States::Motor;
  using States::Limits;

  if ((motor.state == Motor::forwards && limits.state == Limits::left) ||
      (motor.state == Motor::backwards && limits.state == Limits::right)) {
    if (debug_serial) Serial.println("Flipping motor directions!");
    motor.swapDirections();
  }
}

template <bool debug_serial>
void AbsoluteDirectionCalibrator<debug_serial>::breakStall() {
  using States::Motor;

  if (motor.state == Motor::forwards) {
    if (debug_serial) Serial.println("Running the motor backwards to break the stall!");
    motor.backwards();
  } else { // motor.state == Motor::backwards
    if (debug_serial) Serial.println("Running the motor forwards to break the stall!");
    motor.forwards();
  }
}

#endif

