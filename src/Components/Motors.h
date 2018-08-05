#ifndef LPC_Components_Motors_h
#define LPC_Components_Motors_h

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
    Motor(Motors &motors, MotorPort motorPort, bool swapDirections = false);

    void setup();

    int speed = 0;

    void run(int speed);
    void forwards(MotorSpeed speed);
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

namespace States {
  enum class MotorSpeedAdjuster : uint8_t {
    braking, moving
  };
}

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

    using State = States::MotorSpeedAdjuster;

    void setup();
    void update();

    SimpleStateVariable<int> output;
    StateVariable<State> state;

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

#include "Motors.tpp"

#endif

