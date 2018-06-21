#ifndef LPC_Components_DebouncedButton_h
#define LPC_Components_DebouncedButton_h

#include <elapsedMillis.h>

#include "StateVariable.h"

namespace LinearPositionControl { namespace Components {

namespace States {
  enum class DebouncedButton : uint8_t {
    pressed,
    released,
    bouncing
  };
}

class DebouncedButton {
  public:
    DebouncedButton(uint8_t buttonPin, volatile uint8_t &interruptCounter, unsigned int debounceDelay);

    using State = States::DebouncedButton;

    void setup();
    void update();

    StateVariable<State> state;

  private:
    bool setupCompleted = false;

    // Hardware interfaces
    uint8_t buttonPin;

    // Button debouncing
    volatile uint8_t &interruptCounter;
    unsigned int debounceDelay;
    elapsedMillis debounceTimer;
};

} }

#include "DebouncedButton.tpp"

#endif

