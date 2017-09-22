#include <elapsedMillis.h>

class DebouncedButton {
  public:
    DebouncedButton(int buttonPin, volatile uint8_t *interruptCounter, unsigned int debounceDelay);

    void setup();
    void update();
    bool isPressed();

    // Event flags
    bool eventStateChanged = false; // set to true when state changes, must be reset by client
    bool eventStatePressed = false; // set to true when button is pressed, must be reset by client
    bool eventStateReleased = false; // set to true when button is released, must be reset by client

  private:
    int buttonPin;
    volatile uint8_t *interruptCounter;
    unsigned int debounceDelay;
    elapsedMillis debounceTimer;
    bool buttonChanged = false;
    bool buttonState;
};

