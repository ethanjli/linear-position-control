#ifndef Motors_h
#define Motors_h

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

#include "StateVariable.h"

enum MotorPort : uint8_t { // we keep this at global scope to mimic Arduino pin semantics
  M1 = 0, M2 = 1, M3 = 2, M4 = 3
};

namespace LinearPositionControl { namespace Components {

typedef uint8_t MotorSpeed;
typedef uint8_t MotorDirection;

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

    int speed = 255;

    void run(int speed);
    void forwards();
    void forwards(MotorSpeed speed);
    void backwards();
    void backwards(MotorSpeed speed);

    void brake();
    void neutral();

    void swapDirections();
    bool directionsSwapped() const;

  private:
    bool setupCompleted = false;

    Motors &motors;
    const MotorPort motorPort;

    MotorDirection forwardDirection = FORWARD;
    MotorDirection backwardDirection = BACKWARD;
};

class MotorSpeedAdjuster {
  public:
    MotorSpeedAdjuster(
        const int &inputValue, int speedBias, int brakeThreshold
    ) : MotorSpeedAdjuster(
      inputValue, speedBias, -1 * brakeThreshold, brakeThreshold
    ) {}
    MotorSpeedAdjuster(
        const int &inputValue, int speedBias,
        int brakeLowerThreshold, int brakeUpperThreshold
    );

    void setup();
    void update();

    StateVariable<int> output;

    int speedBias;
    int brakeLowerThreshold;
    int brakeUpperThreshold;

    void freeze();
    void unfreeze();

  private:
    const int &input;

    bool frozen = false;
};

} }

#endif

