#ifndef AnalogSensor_h
#define AnalogSensor_h

#include "StateVariable.h"

namespace LinearPositionControl { namespace Components {

class AnalogSensor {
  public:
    AnalogSensor(uint8_t sensorPort);

    void setup();
    void update();

    StateVariable<int> state;

  private:
    bool setupCompleted = false;

    // Hardware interfaces
    uint8_t sensorPin;
};

} }

#endif

