#define NEEDFORSPEED
#define INTERRUPT_FLAG_PIN12 interruptCounter12
#include <EnableInterrupt.h>

//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#include <DebouncedButton.h>

using LinearPositionControl::Components::DebouncedButton;

// Globals

DebouncedButton debouncedButton(12, interruptCounter12, 5);
bool ledState = LOW;

void setup() {
#ifndef DISABLE_LOGGING
  Serial.begin(115200);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
#endif
  debouncedButton.setup();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, ledState);
}

void loop() {
  debouncedButton.update();
  Log.trace("Current button: %d, previous button: %d, current led: %d" CR, debouncedButton.state.current(), debouncedButton.state.previous(), ledState);
  if (debouncedButton.state.previous() == DebouncedButton::State::bouncing &&
      debouncedButton.state.current() == DebouncedButton::State::pressed) {
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
  }
}
