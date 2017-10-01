#define NEEDFORSPEED
#define INTERRUPT_FLAG_PIN12 interruptCounter12
#include <EnableInterrupt.h>

//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#include <DebouncedButton.h>
#include <LED.h>

using LinearPositionControl::Components::DebouncedButton;
using LinearPositionControl::Components::LED;

// Globals

DebouncedButton debouncedButton(12, interruptCounter12, 5);
LED led(LED_BUILTIN);

void setup() {
#ifndef DISABLE_LOGGING
  Serial.begin(115200);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
#endif
  debouncedButton.setup();
  led.setup();
}

void loop() {
  debouncedButton.update();
  led.update();
  if (debouncedButton.state.current() == DebouncedButton::State::bouncing &&
      debouncedButton.state.previous() != DebouncedButton::State::bouncing) {
    led.off();
  } else if (debouncedButton.state.current() == DebouncedButton::State::pressed &&
      debouncedButton.state.previous() == DebouncedButton::State::bouncing) {
    led.highInterval = 800;
    led.lowInterval = 200;
    led.blink();
  } else if (debouncedButton.state.current() == DebouncedButton::State::released &&
      debouncedButton.state.previous() == DebouncedButton::State::bouncing) {
    led.highInterval = 200;
    led.lowInterval = 800;
    led.blink();
  }
}
