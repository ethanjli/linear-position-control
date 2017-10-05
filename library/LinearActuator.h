#ifndef LinearActuator_h
#define LinearActuator_h

#include <elapsedMillis.h>

#include "StateVariable.h"
#include "Motors.h"
#include "Limits.h"
#include "Calibration/Direction.h"

namespace LinearPositionControl {

namespace States {
  enum class LinearActuator : uint8_t {
    readyToCalibrate,
    calibratingDirection,
    calibratingPosition,
    operating
  };
}

template <
  class DirectionCalibrator,
  class PositionCalibrator,
  class MotionController
>
class LinearActuator {
  public:
    LinearActuator(
        DirectionCalibrator &directionCalibrator,
        PositionCalibrator &positionCalibrator,
        MotionController &motionController
    );

    using State = States::LinearActuator;

    void setup();
    void update();

    StateVariable<State> state;

  private:
    bool setupCompleted = false;

    DirectionCalibrator &directionCalibrator;
    PositionCalibrator &positionCalibrator;
    MotionController &motionController;
};

}

#include "LinearActuator.tpp"

#endif

