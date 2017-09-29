#ifndef LinearActuator_h
#define LinearActuator_h

#include <elapsedMillis.h>

#include "Motors.h"
#include "Limits.h"
#include "DirectionCalibration.h"

namespace States {
  enum class LinearActuator : uint8_t {
    calibratingDirection,
    calibratingPosition,
    operating
  };
}

template <class DirectionCalibrator>
class LinearActuator {
  public:
    LinearActuator(
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

