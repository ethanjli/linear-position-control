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

// A motor direction calibrator with an absolute sense of direction - it already knows which end is left and which end is right
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

    elapsedMillis motorStallTimer;
    unsigned int motorStallTimeout = 250;

    void updateUncalibrated();
    void updateCalibrating();
    void updateMotorDirection();
    void breakStall();
    void onDirectionCalibrated();
};

// A calibrator which ensures that the motor can freely move in known directions with multiplexed limit switches - it already knows which direction is forwards and which direction is backwards
template <bool debug_serial>
class MultiplexedDirectionCalibrator {
  public:
    MultiplexedDirectionCalibrator(Motor &motor, MultiplexedLimits &limits);

    void setup();
    void update();

    States::DirectionCalibration state;

  private:
    bool setupCompleted = false;

    Motor &motor;
    AbsoluteLimits &limits;

    elapsedMillis motorStallTimer;
    unsigned int motorStallTimeout = 250;
};

#include "DirectionCalibration.tpp"

#endif

