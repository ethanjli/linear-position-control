#define NEEDFORSPEED
#define INTERRUPT_FLAG_PIN12 interruptCounter12
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

using Actuator = MultiplexedLinearActuator<Components::EdgeCounter, Tracking::Discrete, Motion::Neutral>;
Actuator actuator(shared, M2, 2, interruptCounter2, 12, interruptCounter12);

void setup() {
#ifndef DISABLE_LOGGING
  Serial.begin(115200);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
#endif
  actuator.setup();
}

void loop() {
  actuator.update();
  if (actuator.state.justEntered(Actuator::State::calibratingDirection)) {
    shared.led.lowInterval = 50;
    shared.led.highInterval = 50;
    shared.led.blink();
  } else if (actuator.state.justEntered(Actuator::State::calibratingPosition)) {
    shared.led.lowInterval = 50;
    shared.led.highInterval = 200;
    shared.led.blink();
  } else if (actuator.state.justEntered(Actuator::State::operating)) {
    shared.led.lowInterval = 50;
    shared.led.highInterval = 800;
    shared.led.blink();
  }
}
