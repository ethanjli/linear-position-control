#ifndef LPC_Components_AngleSensor_h
#define LPC_Components_AngleSensor_h

#include <MagneticSensor3D.h>

#include "StateVariable.h"

namespace LinearPositionControl { namespace Components {

class AngleSensor {
  public:
    // Note: currently magnetic3dSensorPort does nothing, because the MagneticSensor3D.h
    // header file defines a singleton global named magnetic3dSensor.
    AngleSensor(
        uint8_t magnetic3dSensorPort, bool swapDirection = false,
        Tlv493d &sensor = magnetic3dSensor, bool accumulate = true
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

    Tlv493d &sensor;
    StateVariable<Position> rawAngle;

    Position overflowDeltaThreshold = 315; // degrees
};

} }

#include "AngleSensor.tpp"

#endif

