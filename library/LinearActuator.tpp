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
  using namespace States::Limits;

  previousState = state;

  leftLimit->update();
  rightLimit->update();

  if (leftLimit->isPressed() && rightLimit->isPressed()) {
    state = BOTH;
  } else if (leftLimit->isPressed()) {
    state = LEFT;
  } else if (rightLimit->isPressed()) {
    state = RIGHT;
  } else {
    state = NONE;
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

  directionCalibrationState = States::DirectionCalibration::UNCALIBRATED;
}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::update() {
  using namespace States::DirectionCalibration;

  limits->update();
  switch (directionCalibrationState) {
    case CALIBRATED:
      updateDirectionCalibrated();
      break;
    case UNCALIBRATED:
      updateDirectionUncalibrated();
      break;
    case CALIBRATING:
      updateDirectionCalibrating();
      break;
  }
}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::runForwards() {
  motors->run(motorPort, forwards, speed);
  motorState = States::Motor::FORWARDS;
}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::runBackwards() {
  motors->run(motorPort, backwards, speed);
  motorState = States::Motor::BACKWARDS;
}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::brake() {
  motors->brake(motorPort);
  motorState = States::Motor::BRAKING;
}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::updateDirectionUncalibrated() {
  using namespace States;

  if (limits->state == Limits::BOTH) return; // delay calibration as long as both limit switches are pressed

  // Start calibration
  directionCalibrationState = DirectionCalibration::CALIBRATING;
  if (debug_serial) Serial.println("Calibrating...");
  motorStallTimer = 0;
  runForwards();
}

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::updateDirectionCalibrating() {
  using namespace States;

  if (limits->state == Limits::BOTH) { // cancel calibration if both limit switches are pressed
    motorState = DirectionCalibration::UNCALIBRATED;
    if (debug_serial) Serial.println("Restarting calibration...");
    return;
  }
  if (limits->state == Limits::NONE) { // let the motor continue to run until it hits a limit
    return;
  }
  if (limits->previousState == Limits::NONE) { // the motor just hit a limit switch
    if (debug_serial) Serial.println("Just hit a limit switch!");
    if ((motorState == Motor::FORWARDS && limits->state == Limits::LEFT) ||
        (motorState == Motor::BACKWARDS && limits->state == Limits::RIGHT)) {
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

template <bool debug_serial>
void AbsoluteLinearActuator<debug_serial>::updateDirectionCalibrated() {
  using namespace States::PositionCalibration;

  switch (positionCalibrationState) {
    case CALIBRATED:
      updatePositionCalibrated();
      break;
    case UNCALIBRATED:
      updatePositionUncalibrated();
      break;
    case CALIBRATING:
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

