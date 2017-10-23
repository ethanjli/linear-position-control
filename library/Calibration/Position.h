#ifndef Calibration_Position_h
#define Calibration_Position_h

#include <elapsedMillis.h>

#include "StateVariable.h"
#include "Motors.h"
#include "Limits.h"
#include "OpticalSensor.h"

namespace LinearPositionControl { namespace Calibration {

namespace States {
  enum class Position : uint8_t {
    calibrated,
    uncalibrated,
    initializing,
    calibrating
  };
}

template<class Limits, class EdgeCounter, class PositionTracker>
class Position {
  public:
    Position(
        Components::Motor &motor,
        Limits &limits,
        EdgeCounter &edgeCounter,
        PositionTracker &positionTracker
    );

    using State = States::Position;

    void setup();
    void update();

    StateVariable<State> state;
    const Components::MotorSpeed calibrationSpeed = 255;
    int expectedNumEdges = -1;

  private:
    bool setupCompleted = false;

  protected:
    Components::Motor &motor;
    Limits &limits;
    EdgeCounter &edgeCounter;
    PositionTracker &positionTracker;

    elapsedMillis limitSwitchTimer;
    unsigned int limitSwitchTimeout = 250;

    int numEdges = 0;

    void updateUncalibrated();
    void updateInitializing();
    void updateCalibrating();
    void onPositionCalibrated();
};

} }

#include "Position.tpp"

#endif

