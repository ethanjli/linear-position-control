#define NEEDFORSPEED
#define INTERRUPT_FLAG_PIN12 interruptCounter12
#define INTERRUPT_FLAG_PIN8 interruptCounter8
#include <EnableInterrupt.h>
#include <DebouncedButton.h>
#include <Motors.h>

// Globals

DebouncedButton left(12, &interruptCounter12, 50);
DebouncedButton right(8, &interruptCounter8, 50);
Motors motors = Motors();
const MotorPort MOTOR_PORT = M2;
const MotorSpeed MOTOR_SPEED = 255;

void setup() {
  left.setup();
  right.setup();
  motors.setup();
  delay(2000);
  motors.run(MOTOR_PORT, FORWARD, MOTOR_SPEED);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  left.update();
  right.update();
  if (left.eventStatePressed) {
    motors.brake(MOTOR_PORT);
    delay(1000);
    motors.run(MOTOR_PORT, FORWARD, MOTOR_SPEED);
    left.eventStatePressed = false;
  }
  if (right.eventStatePressed) {
    motors.brake(MOTOR_PORT);
    delay(1000);
    motors.run(MOTOR_PORT, BACKWARD, MOTOR_SPEED);
    right.eventStatePressed = false;
  }
}
