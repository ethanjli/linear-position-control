#include "DebouncedButton.h"

#define NEEDFORSPEED
#define LIBCALL_ENABLEINTERRUPT
#include <EnableInterrupt.h>

namespace LinearPositionControl { namespace Components {

DebouncedButton::DebouncedButton(
    uint8_t buttonPin,
    volatile uint8_t &interruptCounter,
    unsigned int debounceDelay
) :
  buttonPin(buttonPin),
  interruptCounter(interruptCounter),
  debounceDelay(debounceDelay) {
}

void DebouncedButton::setup() {
  if (setupCompleted) return;

  pinMode(buttonPin, INPUT_PULLUP);
  enableInterruptFast(buttonPin, CHANGE);
  state = States::bouncing;

  setupCompleted = true;
}

void DebouncedButton::update() {
  previousState = state;
  if (interruptCounter) {
    interruptCounter = 0;
    debounceTimer = 0;
    state = States::bouncing;
  }
  if (state == States::bouncing && debounceTimer > debounceDelay) {
    if (!digitalRead(buttonPin)) state = States::pressed;
    else state = States::released;
  }
  if (previousState != state) previousDistinctState = previousState;
}

} }

