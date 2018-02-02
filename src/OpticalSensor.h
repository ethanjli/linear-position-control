#ifndef OpticalSensor_h
#define OpticalSensor_h

#include "StateVariable.h"

namespace LinearPositionControl { namespace Components {

namespace States {
  enum class EdgeCounter : uint8_t {
    unknown,
    dark,
    light
  };
}

class EdgeCounter {
  public:
    EdgeCounter(uint8_t sensorPin, volatile uint8_t &interruptCounter);

    using State = States::EdgeCounter;

    void setup();
    void update();

    StateVariable<State> state;

    void reset();
    uint8_t get();
    uint8_t getAndReset();
    void getAndReset(uint8_t &count);

  private:
    bool setupCompleted = false;

    // Hardware interfaces
    uint8_t sensorPin;

    volatile uint8_t &interruptCounter;
};

} }

#endif
