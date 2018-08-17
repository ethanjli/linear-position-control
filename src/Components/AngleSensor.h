#ifndef LPC_Components_AngleSensor_h
#define LPC_Components_AngleSensor_h

#include <Tlv493d.h>

#include "StateVariable.h"

namespace LinearPositionControl { namespace Components {

const uint8_t kMagnetic3dSensorSwitchPin = 8;

class AngleSensor {
  public:
    // Note: currently magnetic3dSensorPort does nothing
    AngleSensor(
        uint8_t magnetic3dSensorPort, bool swapDirection = false,
        bool accumulate = true
    );

    using Position = float;

    void setup();
    void update();

    void setZero();
    void setReference(Position referencePosition);

    void selectPort();

    SimpleStateVariable<Position> state;

  private:
    bool setupCompleted = false;
    bool swapDirection;
    bool accumulate;

    uint8_t port;
    Tlv493d sensor;
    StateVariable<Position> rawAngle;

    Position overflowDeltaThreshold = 315; // degrees
};

} }

#include "AngleSensor.tpp"

#endif

