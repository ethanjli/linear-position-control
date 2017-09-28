#ifndef LinearActuator_h
#define LinearActuator_h

#include <elapsedMillis.h>

#include "Motors.h"
#include "Limits.h"
#include "DirectionCalibration.h"

namespace States {
  enum class PositionCalibration : uint8_t {
    uncalibrated,
    calibrated,
    calibrating
  };

  enum class LinearActuator : uint8_t {
    calibratingDirection,
    calibratingPosition,
    operating
  };
}

// A linear actuator with an absolute sense of direction - it knows which end is left and which end is right
template <class DirectionCalibrator>
class AbsoluteLinearActuator {
  public:
    AbsoluteLinearActuator(
        DirectionCalibrator &directionCalibrator
    );

    void setup();
    void update();

  private:
    bool setupCompleted = false;

    DirectionCalibrator &directionCalibrator;

    States::LinearActuator state;
};

#include "LinearActuator.tpp"

#endif

