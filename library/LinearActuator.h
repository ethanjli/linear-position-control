#ifndef LinearActuator_h
#define LinearActuator_h

#include <elapsedMillis.h>

#include "DebouncedButton.h"
#include "Motors.h"

namespace States {
  enum class Motor : uint8_t {
    braking,
    forwards,
    backwards
  };

  enum class Limits : uint8_t {
    none,
    left,
    right,
    either,
    both
  };

  enum class DirectionCalibration : uint8_t {
    uncalibrated,
    calibrated,
    calibrating
  };

  enum class PositionCalibration : uint8_t {
    uncalibrated,
    calibrated,
    calibrating
  };
}

// Limit switches with an absolute sense of direction - the left and right switches are distinguishable and known
template <bool debug_serial>
class AbsoluteLimits {
  public:
    AbsoluteLimits(DebouncedButton *leftLimit, DebouncedButton *rightLimit);

    void setup();
    void update();

    States::Limits state;
    States::Limits previousState;

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
    States::Motor motorState;
    States::DirectionCalibration directionCalibrationState;
    States::PositionCalibration positionCalibrationState;

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

