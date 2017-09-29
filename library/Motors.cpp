#include "Motors.h"

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
  brake();

  setupCompleted = true;
}

void Motor::run(int speed) {
  if (speed > 0) {
    motors.run(motorPort, forwardDirection, speed);
    state = States::Motor::forwards;
  } else if (speed < 0) {
    motors.run(motorPort, backwardDirection, -1 * speed);
    state = States::Motor::backwards;
  } else {
    motors.brake(motorPort);
    state = States::Motor::braking;
  }
  updateLastDirection();
}

void Motor::run(MotorDirection direction, MotorSpeed speed) {
  motors.run(motorPort, direction, speed);
  if (speed == 0) {
    motors.brake(motorPort);
    state = States::Motor::braking;
    return;
  }
  switch (direction) {
    case RELEASE:
      motors.neutral(motorPort);
      state = States::Motor::neutral;
      return;
    case FORWARD:
      motors.run(motorPort, direction, speed);
      state = States::Motor::forwards;
      break;
    case BACKWARD:
      motors.run(motorPort, direction, speed);
      state = States::Motor::backwards;
      break;
  }
  updateLastDirection();
}

void Motor::forwards() {
  motors.run(motorPort, forwardDirection, speed);
  state = States::Motor::forwards;
  updateLastDirection();
}

void Motor::forwards(MotorSpeed speed) {
  motors.run(motorPort, forwardDirection, speed);
  state = States::Motor::forwards;
  updateLastDirection();
}

void Motor::backwards() {
  motors.run(motorPort, backwardDirection, speed);
  state = States::Motor::backwards;
  updateLastDirection();
}

void Motor::backwards(MotorSpeed speed) {
  motors.run(motorPort, backwardDirection, speed);
  state = States::Motor::backwards;
  updateLastDirection();
}

void Motor::opposite() {
  if (lastDirection == FORWARD) backwards();
  else if (lastDirection == BACKWARD) forwards();
  updateLastDirection();
}

void Motor::opposite(MotorSpeed speed) {
  if (lastDirection == FORWARD) backwards(speed);
  else if (lastDirection == BACKWARD) forwards(speed);
  updateLastDirection();
}

void Motor::resume() {
  if (lastDirection == FORWARD) forwards();
  else if (lastDirection == BACKWARD) backwards();
}

void Motor::resume(MotorSpeed speed) {
  if (lastDirection == FORWARD) forwards(speed);
  else if (lastDirection == BACKWARD) backwards(speed);
}

void Motor::brake() {
  motors.brake(motorPort);
  state = States::Motor::braking;
}

void Motor::neutral() {
  motors.neutral(motorPort);
  state = States::Motor::neutral;
}

void Motor::swapDirections() {
  MotorDirection temp = forwardDirection;
  forwardDirection = backwardDirection;
  backwardDirection = temp;
  if (lastDirection == FORWARD) lastDirection = BACKWARD;
  else lastDirection = FORWARD; // lastDirection == BACKWARD
}

bool Motor::directionsSwapped() {
  return forwardDirection == BACKWARD;
}

void Motor::updateLastDirection() {
  if (state == States::Motor::forwards) lastDirection = FORWARD;
  else if (state == States::Motor::backwards) lastDirection = BACKWARD;
}

