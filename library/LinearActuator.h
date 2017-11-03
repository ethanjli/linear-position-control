#ifndef LinearActuator_h
#define LinearActuator_h

#include <elapsedMillis.h>

#include "StateVariable.h"

namespace LinearPositionControl {

namespace States {
  enum class LinearActuator : uint8_t {
    readyToCalibrate,
    calibratingDirection,
    calibratingPosition,
    localizingPosition,
    operating
  };
}

template <
  class DirectionCalibrator,
  class LimitsTracker,
  class PositionCalibrator,
  class PositionTracker,
  class MotionController
>
class LinearActuator {
  public:
    LinearActuator(
        DirectionCalibrator &directionCalibrator,
        LimitsTracker &limitsTracker,
        PositionCalibrator &positionCalibrator,
        PositionTracker &positionTracker,
        MotionController &motionController
    );

    using State = States::LinearActuator;

    void setup();
    void update();

    StateVariable<State> state;

  private:
    bool setupCompleted = false;

    DirectionCalibrator &directionCalibrator;
    LimitsTracker &limitsTracker;
    PositionCalibrator &positionCalibrator;
    PositionTracker &positionTracker;
    MotionController &motionController;
};

}

#include "LinearActuator.tpp"

#endif

