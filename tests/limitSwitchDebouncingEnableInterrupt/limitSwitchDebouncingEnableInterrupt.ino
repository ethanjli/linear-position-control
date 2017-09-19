/*
 *  This example shows how to combine the EnableInterrupt
 *  library with some simple debouncing code for reading
 *  button presses consistently.
 *  
 *  The interrupt is attached to digital input 5 and the
 *  service routine just toggles the onboard LED status.
 *  
 *  Tested on: ATmega328P
 *  
 *  Example by Lorenzo Cafaro <lorenzo@ibisco.net>
 *
*/

// Interrupts

#define NEEDFORSPEED

#define INTERRUPT_FLAG_PIN12 interruptCounter12

#include <EnableInterrupt.h>

// Subsystems

#include <elapsedMillis.h>

class DebouncedButton {
  public:
  DebouncedButton(int buttonPin, volatile uint8_t *interruptCounter, unsigned int debounceDelay) :
    buttonPin(buttonPin), interruptCounter(interruptCounter), debounceDelay(debounceDelay) {}

  void setup() {
    pinMode(buttonPin, INPUT_PULLUP);
    enableInterruptFast(buttonPin, RISING);
    buttonState = digitalRead(buttonPin);
  }

  void update() {
    if (*interruptCounter) {
      *interruptCounter = 0;
      debounceTimer = 0;
      buttonChanged = true;
    }
    if (buttonChanged && debounceTimer > debounceDelay) {
      buttonState = digitalRead(buttonPin);
      eventStateChanged = true;
      eventStatePressed = eventStateChanged && buttonState;
      eventStateReleased = eventStateChanged && !buttonState;
      buttonChanged = false;
    }
  }

  bool state() {
    return buttonState;
  }

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

// Globals

DebouncedButton debouncedButton(12, &interruptCounter12, 50);
bool ledState = LOW;

void setup() {
  debouncedButton.setup();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, ledState);
}

void loop() {
  debouncedButton.update();
  if (debouncedButton.eventStatePressed) {
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
    debouncedButton.eventStatePressed = false;
  }
}
