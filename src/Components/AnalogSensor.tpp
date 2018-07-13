#ifndef LPC_Components_AnalogSensor_tpp
#define LPC_Components_AnalogSensor_tpp

namespace LinearPositionControl { namespace Components {

AnalogSensor::AnalogSensor(uint8_t sensorPort, bool swapDirection) :
  sensorPin(sensorPort), swapDirection(swapDirection) {}

void AnalogSensor::setup() {
  if (setupCompleted) return;

  pinMode(sensorPin, INPUT);
  int reading = analogRead(sensorPin);
  if (swapDirection) reading = 1023 - reading;
  state.setup(reading);

  setupCompleted = true;
}

void AnalogSensor::update() {
  int reading = analogRead(sensorPin);
  if (swapDirection) reading = 1023 - reading;
  state.update(reading);
}

void AnalogSensor::setZero() {
}

} }

#endif

