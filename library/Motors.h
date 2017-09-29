#ifndef Motors_h
#define Motors_h

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

namespace States {
  enum class Motor : uint8_t {
    braking,
    neutral,
    forwards,
    backwards
  };
}

class Motors {
  public:
    Motors();

    void setup();
    void run(MotorPort motor_port, MotorDirection direction, MotorSpeed speed);
    void brake(MotorPort motor_port);
    void neutral(MotorPort motor_port);

    Adafruit_DCMotor *motors[4];

  private:
    bool setupCompleted = false;

    // Hardware interfaces
    Adafruit_MotorShield shield;
};

class Motor {
  public:
    Motor(Motors &motors, MotorPort motorPort);

    void setup();
    void update();

    MotorSpeed speed = 255;
    States::Motor state;

    void run(int speed);
    void run(MotorDirection direction, MotorSpeed speed);
    void forwards();
    void forwards(MotorSpeed speed);
    void backwards();
    void backwards(MotorSpeed speed);

    void opposite();
    void opposite(MotorSpeed speed);
    void resume();
    void resume(MotorSpeed speed);

    void brake();
    void neutral();

    void swapDirections();
    bool directionsSwapped();

  private:
    bool setupCompleted = false;

    Motors &motors;
    const MotorPort motorPort;

    MotorDirection forwardDirection = FORWARD;
    MotorDirection backwardDirection = BACKWARD;
    MotorDirection lastDirection = FORWARD;

    void updateLastDirection();
};

#endif

