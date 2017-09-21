#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

typedef uint8_t MotorPort;
const MotorPort M1 = 0;
const MotorPort M2 = 1;
const MotorPort M3 = 2;
const MotorPort M4 = 3;

typedef uint8_t MotorSpeed;
typedef uint8_t MotorDirection;

class Motors {
  public:
    Motors();

    void setup();
    void run(MotorPort motor_port, MotorDirection direction, MotorSpeed speed);
    void brake(MotorPort motor_port);

    Adafruit_MotorShield shield;
    Adafruit_DCMotor *motors[4];
};

