#ifndef LinearActuator_h
#define LinearActuator_h

#include <elapsedMillis.h>

#include "DebouncedButton.h"
#include "Motors.h"

namespace States {
  typedef int State;
  namespace Motor {
    const int BRAKING = 0x00;
    const int FORWARDS = 0x01;
    const int BACKWARDS = 0x02;
  }

  namespace Limits {
    const int NONE = 0x00;
    const int LEFT = 0x01;
    const int RIGHT = 0x02;
    const int EITHER = 0x10;
    const int BOTH = 0x11;
  }

  namespace DirectionCalibration {
    const int UNCALIBRATED = 0x00;
    const int CALIBRATED = 0x01;
    const int CALIBRATING = 0x02;
  }

  namespace PositionCalibration {
    const int UNCALIBRATED = 0x00;
    const int CALIBRATED = 0x01;
    const int CALIBRATING = 0x02;
  }
}

// Limit switches with an absolute sense of direction - the left and right switches are distinguishable and known
template <bool debug_serial>
class AbsoluteLimits {
  public:
    AbsoluteLimits(DebouncedButton *leftLimit, DebouncedButton *rightLimit);

    void setup();
    void update();

    States::State state;
    States::State previousState;

  private:
    DebouncedButton *leftLimit;
    DebouncedButton *rightLimit;
};

template <bool debug_serial>
class MultiplexedLimits {
};

// A linear actuator with an absolute sense of direction - it knows which end is left and which end is right
template <bool debug_serial>
class AbsoluteLinearActuator {
  public:
    AbsoluteLinearActuator(Motors *motors, MotorPort motorPort, AbsoluteLimits<debug_serial> *limits);

    void setup();
    void update();

    // Actuation
    MotorSpeed speed = 255;
    void runForwards();
    void runBackwards();
    void brake();

  private:
    // Actuation
    Motors *motors;
    const MotorPort motorPort;
    MotorDirection forwards = FORWARD;
    MotorDirection backwards = BACKWARD;

    // Sensing
    AbsoluteLimits<debug_serial> *limits;

    // States
    int motorState;
    int limitsState;
    int previousLimitsState;
    int directionCalibrationState;
    int positionCalibrationState;

    // Calibration
    elapsedMillis motorStallTimer;
    unsigned int motorStallTimeout = 250;
    // Direction Calibration
    void updateDirectionUncalibrated();
    void updateDirectionCalibrated();
    void updateDirectionCalibrating();
    // Position Calibration
    void updatePositionUncalibrated();
    void updatePositionCalibrating();
    void updatePositionCalibrated();
};

#include "LinearActuator.tpp"

#endif

