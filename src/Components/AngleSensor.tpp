#ifndef LPC_Components_AngleSensor_tpp
#define LPC_Components_AngleSensor_tpp

namespace LinearPositionControl { namespace Components {

AngleSensor::AngleSensor(
    uint8_t magnetic3dSensorPort, bool swapDirection, bool accumulate
) :
  swapDirection(swapDirection), accumulate(accumulate) {}

void AngleSensor::setup() {
  if (setupCompleted) return;

  sensor.begin();
  delay(sensor.getMeasurementDelay());
  sensor.updateData();
  sensor.setAccessMode(sensor.MASTERCONTROLLEDMODE);
  sensor.disableTemp();
  sensor.updateData();
  Position reading = sensor.getAzimuth() * 180 / PI;
  if (swapDirection) reading *= -1;
  rawAngle.setup(reading);
  state.setup(rawAngle.current);

  setupCompleted = true;
}

void AngleSensor::setZero() {
  state.update(0);
}

void AngleSensor::setReference(Position referencePosition) {
  state.update(referencePosition);
}

void AngleSensor::update() {
  if (rawAngle.currentDuration() > sensor.getMeasurementDelay()) {
    sensor.updateData();
  } else {
    return;
  }
  Position reading = sensor.getAzimuth() * 180 / PI;
  if (swapDirection) reading *= -1;
  rawAngle.update(reading);

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

