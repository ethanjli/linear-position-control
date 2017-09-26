#ifndef LinearActuator_tpp
#define LinearActuator_tpp

// AbsoluteLimits
template <bool debug_serial>
AbsoluteLimits<debug_serial>::AbsoluteLimits(DebouncedButton *leftLimit, DebouncedButton *rightLimit) :
  leftLimit(leftLimit), rightLimit(rightLimit) {}

template <bool debug_serial>
void AbsoluteLimits<debug_serial>::setup() {
  leftLimit->setup();
  rightLimit->setup();

  update();
  previousState = state;
}

template <bool debug_serial>
void AbsoluteLimits<debug_serial>::update() {
  using States::Limits;

  previousState = state;

  leftLimit->update();
  rightLimit->update();

  if (leftLimit->isPressed() && rightLimit->isPressed()) {
    state = Limits::both;
  } else if (leftLimit->isPressed()) {
    state = Limits::left;
  } else if (rightLimit->isPressed()) {
    state = Limits::right;
  } else {
    state = Limits::none;
  }
}


// AbsoluteLinearActuator

template <bool debug_serial>
AbsoluteLinearActuator<debug_serial>::AbsoluteLinearActuator(Motors *motors, MotorPort motorPort, AbsoluteLimits<debug_serial> *limits) :
  motors(motors), motorPort(motorPort), limits(limits) {}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::setup() {
  motors->setup();
  brake();

  limits->setup();

  directionCalibrationState = States::DirectionCalibration::uncalibrated;
}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::update() {
  using States::DirectionCalibration;

  limits->update();
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
void AbsoluteLinearActuator<debug_serial>::runForwards() {
  motors->run(motorPort, forwards, speed);
  motorState = States::Motor::forwards;
}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::runBackwards() {
  motors->run(motorPort, backwards, speed);
  motorState = States::Motor::backwards;
}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::brake() {
  motors->brake(motorPort);
  motorState = States::Motor::braking;
}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::updateDirectionUncalibrated() {
  using States::Limits;
  using States::DirectionCalibration;

  if (limits->state == Limits::both) return; // delay calibration as long as both limit switches are pressed

  // Start calibration
  directionCalibrationState = DirectionCalibration::calibrating;
  if (debug_serial) Serial.println("Calibrating...");
  motorStallTimer = 0;
  runForwards();
}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::updateDirectionCalibrating() {
  using States::Motor;
  using States::Limits;
  using States::DirectionCalibration;

  if (limits->state == Limits::both) { // cancel calibration if both limit switches are pressed
    directionCalibrationState = DirectionCalibration::uncalibrated;
    if (debug_serial) Serial.println("Restarting calibration...");
    return;
  }
  if (limits->state == Limits::none) { // let the motor continue to run until it hits a limit
    return;
  }
  if (limits->previousState == Limits::none) { // the motor just hit a limit switch
    if (debug_serial) Serial.println("Just hit a limit switch!");
    if ((motorState == Motor::forwards && limits->state == Limits::left) ||
        (motorState == Motor::backwards && limits->state == Limits::right)) {
      if (debug_serial) Serial.println("Flipping directions!");
      forwards = BACKWARD;
      backwards = FORWARD;
    }
    runForwards();
    directionCalibrationState = DirectionCalibration::calibrated;
    if (debug_serial) {
      Serial.println("Calibrated!");
      if (forwards == BACKWARD) Serial.print("We flipped the motor's pins in software.");
      else Serial.print("We don't need to flip the motor's pins.");
    }
    digitalWrite(LED_BUILTIN, HIGH);
    return;
  }
  if (motorStallTimer > motorStallTimeout) {
    if (debug_serial) Serial.println("Stall detected!");
    if (motorState == Motor::forwards) {
      if (debug_serial) Serial.println("Running the motor backwards to break the stall!");
      runBackwards();
    } else { // motorState == Motor::backwards
      if (debug_serial) Serial.println("Running the motor forwards to break the stall!");
      runForwards();
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

