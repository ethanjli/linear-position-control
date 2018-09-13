#ifndef LPC_Components_AngleSensor_tpp
#define LPC_Components_AngleSensor_tpp

#include <Wire.h>

namespace LinearPositionControl { namespace Components {

// AS5601

void AS5601::setup() {
  if (setupCompleted) return;

  Wire.begin();
  setRegisterAddressPointer(RegisterAddress::RawAngleHigh);
  readAngle();
}

void AS5601::update() {
  readAngle();
}

bool AS5601::updatedReading() const {
  return readingUpdated;
}

uint16_t AS5601::getReading() const {
  return reading;
}

float AS5601::getAngle() const {
  return scaleReading(reading);
}

void AS5601::setRegisterAddressPointer(RegisterAddress addr) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(static_cast<uint8_t>(addr));
  Wire.endTransmission();
}

bool AS5601::getByte(uint8_t &byte) {
  Wire.requestFrom(deviceAddress, static_cast<uint8_t>(1));
  if (Wire.available() < 1) return false;
  byte = 0xFF & Wire.read();
  return true;
}

void AS5601::readAngle() {
  uint8_t highByte;
  uint8_t lowByte;
  if (!getByte(highByte) || !getByte(lowByte)) {
    readingUpdated = false;
    return;
  }
  reading = word(highByte, lowByte);
  readingUpdated = true;
}

float AS5601::scaleReading(uint16_t reading) const {
  return (reading / 4095.0) * 360;
}

// AngleSensor

AngleSensor::AngleSensor(
    uint8_t angleSensorPort, bool swapDirection, bool accumulate
) :
  swapDirection(swapDirection), accumulate(accumulate), port(angleSensorPort) {}

void AngleSensor::setup() {
  if (setupCompleted) return;

  pinMode(kAngleSensorSwitchPin, OUTPUT);
  selectPort();
  sensor.setup();
  Position angle = sensor.getAngle();
  if (swapDirection) angle *= -1;
  rawAngle.setup(angle);
  state.setup(rawAngle.current);

  setupCompleted = true;
}

void AngleSensor::setZero() {
  setReference(0);
}

void AngleSensor::setReference(Position referencePosition) {
  // Reset position delta to 0
  update();
  update();
  // Reset current position
  state.update(referencePosition);
}

void AngleSensor::selectPort() {
  if (port) digitalWrite(kAngleSensorSwitchPin, HIGH);
  else digitalWrite(kAngleSensorSwitchPin, LOW);
}

void AngleSensor::update() {
  selectPort();
  sensor.update();
  Position angle = sensor.getAngle();
  if (swapDirection) angle *= -1;
  rawAngle.update(angle);

  if (!accumulate) {
    state.update(rawAngle.current);
    return;
  }

  Position delta = rawAngle.current - rawAngle.previous;
  if (abs(delta) > overflowDeltaThreshold) {
    int deltaSign = (0.0f < delta) - (delta < 0.0f);
    delta -= deltaSign * 360.0f;
  }
  state.update(state.current + delta);
}

} }

#endif

