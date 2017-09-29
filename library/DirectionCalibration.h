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

template<class Limits>
class DirectionCalibrator {
  public:
    DirectionCalibrator(Motor &motor, Limits &limits);

    void setup();
    void update();

    States::DirectionCalibration state;

  private:
    bool setupCompleted = false;

  protected:
    Motor &motor;
    Limits &limits;

    elapsedMillis motorStallTimer;
    unsigned int motorStallTimeout = 250;

    void updateUncalibrated();
    void updateCalibrating();
    void updateMotorDirection();
    void onDirectionCalibrated();
};

#include "DirectionCalibration.tpp"

#endif

