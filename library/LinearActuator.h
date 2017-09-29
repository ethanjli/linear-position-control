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

template <class DirectionCalibrator, class MotionController>
class LinearActuator {
  public:
    LinearActuator(
        DirectionCalibrator &directionCalibrator,
        MotionController &motionController
    );

    void setup();
    void update();

  private:
    bool setupCompleted = false;

    DirectionCalibrator &directionCalibrator;
    MotionController &motionController;

    States::LinearActuator state;
};

#include "LinearActuator.tpp"

#endif

