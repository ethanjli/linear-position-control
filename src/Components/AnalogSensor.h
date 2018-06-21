#ifndef AnalogSensor_h
#define AnalogSensor_h

#include "StateVariable.h"

namespace LinearPositionControl { namespace Components {

class AnalogSensor {
  public:
    AnalogSensor(uint8_t sensorPort);

    using Position = int;

    void setup();
    void update();

    void setZero();

    SimpleStateVariable<Position> state;

  private:
    bool setupCompleted = false;

    // Hardware interfaces
    uint8_t sensorPin;
};

} }

#include "AnalogSensor.tpp"

#endif

