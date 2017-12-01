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

  pinMode(sensorPin, INPUT_PULLUP);
  interruptCounter = 0;
  enableInterruptFast(sensorPin, CHANGE);
  if (digitalRead(sensorPin)) state.setup(State::light);
  else state.setup(State::dark);

  setupCompleted = true;
}

void EdgeCounter::update() {
  if (digitalRead(sensorPin)) state.update(State::light);
  else state.update(State::dark);
}

void EdgeCounter::reset() {
  disableInterrupt(sensorPin);
  interruptCounter = 0;
  enableInterruptFast(sensorPin, CHANGE);
}

uint8_t EdgeCounter::get() {
  return interruptCounter;
}

uint8_t EdgeCounter::getAndReset() {
  disableInterrupt(sensorPin);
  uint8_t counter = interruptCounter;
  interruptCounter = 0;
  enableInterruptFast(sensorPin, CHANGE);
  return counter;
}

void EdgeCounter::getAndReset(uint8_t &count) {
  disableInterrupt(sensorPin);
  count = interruptCounter;
  interruptCounter = 0;
  enableInterruptFast(sensorPin, CHANGE);
}

} }

