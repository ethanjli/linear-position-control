#include "Motors.h"

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

Motor::Motor(Motors &motors, MotorPort motorPort) :
  motors(motors), motorPort(motorPort) {
}

void Motor::setup() {
  if (setupCompleted) return;

  motors.setup();
  motors.brake(motorPort);
  state.setup(State::braking);

  setupCompleted = true;
}

void Motor::run(int speed) {
  if (speed > 0) {
    forwards(speed);
  } else if (speed < 0) {
    backwards(-1 * speed);
  } else {
    brake();
  }
}

void Motor::forwards() {
  motors.run(motorPort, forwardDirection, speed);
  state.update(State::forwards);
  updateLastDirection();
}

void Motor::forwards(MotorSpeed speed) {
  motors.run(motorPort, forwardDirection, speed);
  state.update(State::forwards);
  updateLastDirection();
}

void Motor::backwards() {
  motors.run(motorPort, backwardDirection, speed);
  state.update(State::backwards);
  updateLastDirection();
}

void Motor::backwards(MotorSpeed speed) {
  motors.run(motorPort, backwardDirection, speed);
  state.update(State::backwards);
  updateLastDirection();
}

void Motor::opposite() {
  if (lastDirection == FORWARD) backwards();
  else if (lastDirection == BACKWARD) forwards();
}

void Motor::opposite(MotorSpeed speed) {
  if (lastDirection == FORWARD) backwards(speed);
  else if (lastDirection == BACKWARD) forwards(speed);
}

void Motor::resume() {
  if (lastDirection == FORWARD) forwards();
  else if (lastDirection == BACKWARD) backwards();
}

void Motor::resume(MotorSpeed speed) {
  if (lastDirection == FORWARD) forwards(speed);
  else if (lastDirection == BACKWARD) backwards(speed);
}

MotorDirection Motor::resumeDirection() const {
  return lastDirection;
}

void Motor::brake() {
  motors.brake(motorPort);
  state.update(State::braking);
}

void Motor::neutral() {
  motors.neutral(motorPort);
  state.update(State::neutral);
}

void Motor::swapDirections() {
  MotorDirection temp = forwardDirection;
  forwardDirection = backwardDirection;
  backwardDirection = temp;
  if (lastDirection == FORWARD) lastDirection = BACKWARD;
  else lastDirection = FORWARD; // lastDirection == BACKWARD
}

bool Motor::directionsSwapped() const {
  return forwardDirection == BACKWARD;
}

void Motor::updateLastDirection() {
  if (state.current() == State::forwards) lastDirection = FORWARD;
  else if (state.current() == State::backwards) lastDirection = BACKWARD;
}

// MotorSpeedAdjuster

MotorSpeedAdjuster::MotorSpeedAdjuster(
    StateVariable<int> &inputStateVariable, int speedBias, int brakeThreshold
) :
  input(inputStateVariable), speedBias(speedBias), brakeThreshold(brakeThreshold) {}

void MotorSpeedAdjuster::setup() {
  output.setup(0);
}

void MotorSpeedAdjuster::update() {
  int adjusted = input.current();
  adjusted += speedBias;
  if (abs(adjusted) < brakeThreshold) adjusted = 0;
  output.update(adjusted);
}

} }

