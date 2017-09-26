#ifndef LinearActuator_h
#define LinearActuator_h

#include <elapsedMillis.h>

#include "Motors.h"
#include "Limits.h"

namespace States {
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

// A linear actuator with an absolute sense of direction - it knows which end is left and which end is right
template <bool debug_serial>
class AbsoluteLinearActuator {
  public:
    AbsoluteLinearActuator(Motor &motor, AbsoluteLimits &limits);

    void setup();
    void update();

  private:
    bool setupCompleted = false;

    Motor &motor;
    AbsoluteLimits &limits;

    // States
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

