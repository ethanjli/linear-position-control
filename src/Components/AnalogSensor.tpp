#ifndef LPC_Components_AnalogSensor_tpp
#define LPC_Components_AnalogSensor_tpp

namespace LinearPositionControl { namespace Components {

AnalogSensor::AnalogSensor(uint8_t sensorPort) :
  sensorPin(sensorPort)
{
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

void AnalogSensor::setZero() {
}

} }

#endif

