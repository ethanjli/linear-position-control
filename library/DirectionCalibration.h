#ifndef DirectionCalibration_h
#define DirectionCalibration_h

#include <elapsedMillis.h>

#include "Motors.h"
#include "Limits.h"

namespace States {
  enum class DirectionCalibration : uint8_t {
    calibrated,
    uncalibrated,
    calibrating
  };
}

// A motor direction calibrator with an absolute sense of direction - it knows which end is left and which end is right
template <bool debug_serial>
class AbsoluteDirectionCalibrator {
  public:
    AbsoluteDirectionCalibrator(Motor &motor, AbsoluteLimits &limits);

    void setup();
    void update();

    States::DirectionCalibration state;

  private:
    bool setupCompleted = false;

    Motor &motor;
    AbsoluteLimits &limits;

    // Calibration
    elapsedMillis motorStallTimer;
    unsigned int motorStallTimeout = 250;
    // Direction Calibration
    void updateDirectionUncalibrated();
    void updateDirectionCalibrating();
    void updateMotorDirection();
    void breakStall();
    void onDirectionCalibrated();
};

#include "DirectionCalibration.tpp"

#endif

