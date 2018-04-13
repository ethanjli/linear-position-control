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

    using State = States::Motor;

    void setup();

    MotorSpeed speed = 255;
    StateVariable<State> state;

    void run(int speed);
    void forwards();
    void forwards(MotorSpeed speed);
    void backwards();
    void backwards(MotorSpeed speed);

    void opposite();
    void opposite(MotorSpeed speed);
    void resume();
    void resume(MotorSpeed speed);
    MotorDirection resumeDirection() const;

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
    MotorDirection lastDirection = FORWARD;

    void updateLastDirection();
};

class MotorSpeedAdjuster {
  public:
    MotorSpeedAdjuster(
        StateVariable<int> &inputStateVariable, int speedBias, int brakeThreshold
    ) : MotorSpeedAdjuster(
      inputStateVariable, speedBias, -1 * brakeThreshold, brakeThreshold
    ) {}
    MotorSpeedAdjuster(
        StateVariable<int> &inputStateVariable, int speedBias,
        int brakeLowerThreshold, int brakeUpperThreshold
    );

    void setup();
    void update();

    StateVariable<int> output;

    int speedBias;
    int brakeLowerThreshold;
    int brakeUpperThreshold;

    void freeze(bool brake = true);
    void unfreeze();

  private:
    StateVariable<int> &input;

    bool frozen = false;
};

} }

#endif

