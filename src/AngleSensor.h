#ifndef AngleSensor_h
#define AngleSensor_h

#include <MagneticSensor3D.h>

#include "StateVariable.h"

namespace LinearPositionControl { namespace Components {

class AngleSensor {
  public:
    AngleSensor(Tlv493d &sensor = magnetic3dSensor);

    void setup();
    void update();

    StateVariable<float> state;

  private:
    bool setupCompleted = false;

    Tlv493d &sensor;
};

} }

#endif

