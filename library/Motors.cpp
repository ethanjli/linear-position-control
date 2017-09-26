#include "Motors.h"

// Motors

Motors::Motors() :
  shield(Adafruit_MotorShield()) {}

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

// Motor

Motor::Motor(Motors &motors, MotorPort motorPort) :
  motors(motors), motorPort(motorPort) {}

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
}

void Motor::forwards() {
  motors.run(motorPort, forwardDirection, speed);
  state = States::Motor::forwards;
}

void Motor::forwards(MotorSpeed speed) {
  motors.run(motorPort, forwardDirection, speed);
  state = States::Motor::forwards;
}

void Motor::backwards() {
  motors.run(motorPort, backwardDirection, speed);
  state = States::Motor::backwards;
}

void Motor::backwards(MotorSpeed speed) {
  motors.run(motorPort, backwardDirection, speed);
  state = States::Motor::backwards;
}

void Motor::brake() {
  motors.brake(motorPort);
  state = States::Motor::braking;
}

void Motor::swapDirections() {
  MotorDirection temp = forwardDirection;
  forwardDirection = backwardDirection;
  backwardDirection = temp;
}

bool Motor::directionsSwapped() {
  return forwardDirection == BACKWARD;
}

