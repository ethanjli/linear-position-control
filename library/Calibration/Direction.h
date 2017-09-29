#ifndef Calibration_Direction_h
#define Calibration_Direction_h

#include <elapsedMillis.h>

#include "Motors.h"
#include "Limits.h"

namespace LinearPositionControl { namespace Calibration {

namespace States {
  enum class Direction : uint8_t {
    calibrated,
    uncalibrated,
    calibrating
  };
}

template<class Limits>
class Direction {
  public:
    Direction(Components::Motor &motor, Limits &limits);

    using States = States::Direction;

    void setup();
    void update();

    States state;

  private:
    bool setupCompleted = false;

  protected:
    Components::Motor &motor;
    Limits &limits;

    elapsedMillis motorStallTimer;
    unsigned int motorStallTimeout = 250;

    void updateUncalibrated();
    void updateCalibrating();
    void updateMotorDirection();
    void onDirectionCalibrated();
};

} }

#include "Direction.tpp"

#endif

