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

template <class DirectionCalibrator, class MotionController>
class LinearActuator {
  public:
    LinearActuator(
        DirectionCalibrator &directionCalibrator,
        MotionController &motionController
    );

    using State = States::LinearActuator;

    void setup();
    void update();

  private:
    bool setupCompleted = false;

    DirectionCalibrator &directionCalibrator;
    MotionController &motionController;

    StateVariable<State> state;
};

}

#include "LinearActuator.tpp"

#endif

