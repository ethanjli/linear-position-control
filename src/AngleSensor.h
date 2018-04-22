#ifndef AngleSensor_h
#define AngleSensor_h

#include <MagneticSensor3D.h>

#include "StateVariable.h"

namespace LinearPositionControl { namespace Components {

class AngleSensor {
  public:
    // Note: currently magnetic3dSensorPort does nothing, because the MagneticSensor3D.h
    // header file defines a singleton global named magnetic3dSensor.
    AngleSensor(uint8_t magnetic3dSensorPort, Tlv493d &sensor = magnetic3dSensor, bool accumulate = true);

    void setup();
    void update();

    void setZero();

    StateVariable<float> state;

  private:
    bool setupCompleted = false;
    bool accumulate;

    Tlv493d &sensor;
    StateVariable<float> rawAngle;

    float overflowDeltaThreshold = 315; // degrees
};

} }

#endif

