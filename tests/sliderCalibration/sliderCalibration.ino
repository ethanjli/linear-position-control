#define NEEDFORSPEED
#define INTERRUPT_FLAG_PIN12 interruptCounter12
#define INTERRUPT_FLAG_PIN8 interruptCounter8
#define INTERRUPT_FLAG_PIN2 interruptCounter2
#include <EnableInterrupt.h>

//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#include <LinearPositionControl.h>
#include <OpticalSensor.h>
#include <Tracking/Discrete.h>
#include <Motion/Neutral.h>
using namespace LinearPositionControl;

// Singletons

SharedComponents shared;

// Globals

using Actuator = UnmultiplexedLinearActuator<Components::EdgeCounter, Tracking::Discrete, Motion::Neutral>;
Actuator actuator(shared, M2, 2, interruptCounter2, 8, interruptCounter8, 12, interruptCounter12);

void setup() {
#ifndef DISABLE_LOGGING
  Serial.begin(115200);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
#endif
  shared.led.setup();
  actuator.actuator.setup();
}

void loop() {
  actuator.actuator.update();
  shared.led.update();
  if (actuator.actuator.state.justEntered(Actuator::Actuator::State::calibratingDirection)) {
    shared.led.lowInterval = 50;
    shared.led.highInterval = 50;
    shared.led.blink();
  } else if (actuator.actuator.state.justEntered(Actuator::Actuator::State::calibratingPosition)) {
    shared.led.lowInterval = 50;
    shared.led.highInterval = 200;
    shared.led.blink();
  } else if (actuator.actuator.state.justEntered(Actuator::Actuator::State::operating)) {
    shared.led.lowInterval = 50;
    shared.led.highInterval = 800;
    shared.led.blink();
  }
}
