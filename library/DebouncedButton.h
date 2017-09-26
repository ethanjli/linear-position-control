#ifndef DebouncedButton_h
#define DebouncedButton_h

#include <elapsedMillis.h>

class DebouncedButton {
  public:
    DebouncedButton(uint8_t buttonPin, volatile uint8_t &interruptCounter, unsigned int debounceDelay);

    void setup();
    void update();

    // Button state
    bool isPressed();
    // Event flags
    bool eventStateChanged = false; // set to true when state changes, must be reset by client
    bool eventStatePressed = false; // set to true when button is pressed, must be reset by client
    bool eventStateReleased = false; // set to true when button is released, must be reset by client
    void clearEventFlags();

  private:
    bool setupCompleted = false;

    // Hardware interfaces
    uint8_t buttonPin;

    // Button debouncing
    volatile uint8_t &interruptCounter;
    unsigned int debounceDelay;
    elapsedMillis debounceTimer;

    // Button state
    bool buttonChanged = false;
    bool buttonState;
};

#endif

