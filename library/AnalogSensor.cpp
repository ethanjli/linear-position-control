#include "AnalogSensor.h"

#define NEEDFORSPEED
#define LIBCALL_ENABLEINTERRUPT
#include <EnableInterrupt.h>

namespace LinearPositionControl { namespace Components {

AnalogSensor::AnalogSensor(uint8_t sensorPin) {
}

void AnalogSensor::setup() {
  if (setupCompleted) return;

  pinMode(sensorPin, INPUT);
  state.setup(analogRead(sensorPin));

  setupCompleted = true;
}

void AnalogSensor::update() {
  state.update(analogRead(sensorPin));
}


} }
