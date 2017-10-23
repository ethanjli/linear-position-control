//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#define SENSOR_PIN2
#define SENSOR_PIN12
#include <LinearPositionControl.h>
#include <OpticalSensor.h>
#include <Tracking/Discrete.h>
#include <Motion/Oscillator.h>
using namespace LinearPositionControl;

// Singletons

SharedComponents shared;

// Globals

using Actuator = MultiplexedLinearActuator<Components::EdgeCounter, Tracking::Discrete, Motion::Oscillator>;
Actuator actuator(shared, M2, 2, 12);

void setup() {
#ifndef DISABLE_LOGGING
  Serial.begin(115200);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
#endif
  actuator.setup();
  actuator.motor.speed = 63;
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
