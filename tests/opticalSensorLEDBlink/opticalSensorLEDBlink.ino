#define NEEDFORSPEED
#define INTERRUPT_FLAG_PIN2 interruptCounter2
#include <EnableInterrupt.h>

//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#include <OpticalSensor.h>
#include <LED.h>

using LinearPositionControl::Components::EdgeCounter;
using LinearPositionControl::Components::LED;

// Globals

EdgeCounter edgeCounter(2, interruptCounter2);
LED led(LED_BUILTIN);

void setup() {
#ifndef DISABLE_LOGGING
  Serial.begin(115200);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
#endif
  edgeCounter.setup();
  led.setup();
  led.periods = 0;
  led.highInterval = 100;
  led.lowInterval = 100;
  led.blink();
}

void loop() {
  edgeCounter.update();
  led.update();
  led.periods += edgeCounter.getAndReset();
  if (led.state.current() == LED::State::off) led.blink();
}
