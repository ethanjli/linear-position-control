#ifndef DebouncedButton_h
#define DebouncedButton_h

#include <elapsedMillis.h>

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

    using States = States::DebouncedButton;

    void setup();
    void update();

    States state;
    States previousState;
    States previousDistinctState;

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

#endif

