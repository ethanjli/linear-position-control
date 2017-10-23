#include "OpticalSensor.h"

#define NEEDFORSPEED
#define LIBCALL_ENABLEINTERRUPT
#include <EnableInterrupt.h>

namespace LinearPositionControl { namespace Components {

EdgeCounter::EdgeCounter(uint8_t sensorPin, volatile uint8_t &interruptCounter) :
  sensorPin(sensorPin), interruptCounter(interruptCounter) {
}

void EdgeCounter::setup() {
  if (setupCompleted) return;

  pinMode(sensorPin, INPUT);
  interruptCounter = 0;
  enableInterruptFast(sensorPin, CHANGE);
  if (digitalRead(sensorPin)) state.setup(State::light);
  else state.setup(State::dark);

  setupCompleted = true;
}

void EdgeCounter::update() {
  if (digitalRead(sensorPin)) state.update(State::light);
  else state.update(State::dark);

  //uint8_t counter = interruptCounter;
  //if (counter) Serial.println(counter);
}

uint8_t EdgeCounter::getAndReset() {
  disableInterrupt(sensorPin);
  uint8_t counter = interruptCounter;
  interruptCounter = 0;
  enableInterruptFast(sensorPin, CHANGE);
  return counter;
}

} }

