#include "DebouncedButton.h"
#define NEEDFORSPEED
#define LIBCALL_ENABLEINTERRUPT
#include <EnableInterrupt.h>

DebouncedButton::DebouncedButton(uint8_t buttonPin, volatile uint8_t *interruptCounter, unsigned int debounceDelay) :
  buttonPin(buttonPin), interruptCounter(interruptCounter), debounceDelay(debounceDelay) {}

void DebouncedButton::setup() {
  if (setupCompleted) return;
  pinMode(buttonPin, INPUT_PULLUP);
  enableInterruptFast(buttonPin, FALLING);
  buttonState = digitalRead(buttonPin);
  eventStatePressed = buttonState;
  eventStateReleased = !buttonState;
}

void DebouncedButton::update() {
  if (*interruptCounter) {
    *interruptCounter = 0;
    debounceTimer = 0;
    buttonChanged = true;
  }
  if (buttonChanged && debounceTimer > debounceDelay) {
    buttonState = digitalRead(buttonPin);
    eventStateChanged = true;
    eventStatePressed = eventStateChanged && !buttonState;
    eventStateReleased = eventStateChanged && buttonState;
    buttonChanged = false;
  }
}

bool DebouncedButton::isPressed() {
  return !buttonState;
}

void DebouncedButton::clearEventFlags() {
  eventStateChanged = false;
  eventStatePressed = false;
  eventStateReleased = false;
}

