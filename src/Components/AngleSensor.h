#ifndef LPC_Components_AngleSensor_h
#define LPC_Components_AngleSensor_h

#include <Tlv493d.h>

#include "StateVariable.h"

namespace LinearPositionControl { namespace Components {

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

    SimpleStateVariable<Position> state;

  private:
    bool setupCompleted = false;
    bool swapDirection;
    bool accumulate;

    Tlv493d sensor;
    StateVariable<Position> rawAngle;

    Position overflowDeltaThreshold = 315; // degrees
};

} }

#include "AngleSensor.tpp"

#endif

