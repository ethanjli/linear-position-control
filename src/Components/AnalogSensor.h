#ifndef LPC_Components_AnalogSensor_h
#define LPC_Components_AnalogSensor_h

#include "StateVariable.h"

namespace LinearPositionControl { namespace Components {

class AnalogSensor {
  public:
    AnalogSensor(uint8_t sensorPort, bool swapDirection = true);

    using Position = int;

    void setup();
    void update();

    void setZero();

    SimpleStateVariable<Position> state;

  private:
    bool setupCompleted = false;

    // Hardware interfaces
    uint8_t sensorPin;
    bool swapDirection;
};

} }

#include "AnalogSensor.tpp"

#endif

