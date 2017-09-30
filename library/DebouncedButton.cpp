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
  state.setup(State::bouncing);

  setupCompleted = true;
}

void DebouncedButton::update() {
  if (interruptCounter) { // we've sensed a button press or a bounce
    interruptCounter = 0;
    debounceTimer = 0;
    state.update(State::bouncing);
  }
  if (debounceTimer > debounceDelay) { // we're confident that the button is no longer bouncing
    if (!digitalRead(buttonPin)) state.update(State::pressed);
    else state.update(State::released);
  }
}

} }

