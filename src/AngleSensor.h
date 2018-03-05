#ifndef AngleSensor_h
#define AngleSensor_h

#include <MagneticSensor3D.h>

#include "StateVariable.h"

namespace LinearPositionControl { namespace Components {

class AngleSensor {
  public:
    AngleSensor(Tlv493d &sensor = magnetic3dSensor, bool accumulate = true);

    void setup();
    void update();

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

