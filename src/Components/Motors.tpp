#ifndef LPC_Components_Motors_tpp
#define LPC_Components_Motors_tpp

namespace LinearPositionControl { namespace Components {

// Motors

Motors::Motors() :
  shield(Adafruit_MotorShield()) {
}

void Motors::setup() {
  if (setupCompleted) return;

  shield.begin();
  for (uint8_t motor_port = 1; motor_port <= 4; ++motor_port) {
    motors[motor_port - 1] = shield.getMotor(motor_port);
  }

  setupCompleted = true;
}

void Motors::run(MotorPort motor_port, MotorDirection direction, MotorSpeed speed) {
  motors[motor_port]->run(direction);
  motors[motor_port]->setSpeed(speed);
}

void Motors::brake(MotorPort motor_port) {
  motors[motor_port]->setSpeed(0);
}

void Motors::neutral(MotorPort motor_port) {
  motors[motor_port]->run(RELEASE);
}

// Motor

Motor::Motor(Motors &motors, MotorPort motorPort, bool swapDirections) :
  motors(motors), motorPort(motorPort) {
    if (swapDirections) {
      forwardDirection = BACKWARD;
      backwardDirection = FORWARD;
    }
}

void Motor::setup() {
  if (setupCompleted) return;

  motors.setup();
  motors.brake(motorPort);

  setupCompleted = true;
}

void Motor::run(int newSpeed) {
  if (newSpeed > 0) {
    forwards(newSpeed);
  } else if (newSpeed < 0) {
    backwards(-1 * newSpeed);
  } else {
    brake();
  }
}

void Motor::forwards(MotorSpeed newSpeed) {
  motors.run(motorPort, forwardDirection, newSpeed);
  speed = newSpeed;
}

void Motor::backwards(MotorSpeed newSpeed) {
  motors.run(motorPort, backwardDirection, newSpeed);
  speed = newSpeed;
  speed *= -1;
}

void Motor::brake() {
  speed = 0;
  motors.brake(motorPort);
}

void Motor::neutral() {
  speed = 0;
  motors.neutral(motorPort);
}

void Motor::swapDirections() {
  MotorDirection temp = forwardDirection;
  forwardDirection = backwardDirection;
  backwardDirection = temp;
}

bool Motor::directionsSwapped() const {
  return forwardDirection == BACKWARD;
}

// MotorSpeedAdjuster

MotorSpeedAdjuster::MotorSpeedAdjuster(
    const int &inputValue, int speedBias,
    int brakeLowerThreshold, int brakeUpperThreshold
) :
  input(inputValue), speedBias(speedBias),
  brakeLowerThreshold(brakeLowerThreshold),
  brakeUpperThreshold(brakeUpperThreshold) {}

void MotorSpeedAdjuster::setup() {
  output.setup(0);
  state.setup(State::braking);
}

void MotorSpeedAdjuster::update() {
  if (frozen) {
    output.update(output.current);
    return;
  }
  int adjusted = input + speedBias;
  if (adjusted > brakeLowerThreshold && adjusted < brakeUpperThreshold) adjusted = 0;
  if (adjusted != 0) state.update(State::moving);
  else state.update(State::braking);
  output.update(adjusted);
}

void MotorSpeedAdjuster::freeze() {
  frozen = true;
}

void MotorSpeedAdjuster::unfreeze() {
  frozen = false;
}

} }

#endif

