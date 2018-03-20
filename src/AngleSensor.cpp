#include "AngleSensor.h"

namespace LinearPositionControl { namespace Components {

AngleSensor::AngleSensor(Tlv493d &sensor, bool accumulate) :
  sensor(sensor), accumulate(accumulate)
{
}

void AngleSensor::setup() {
  if (setupCompleted) return;

  sensor.begin();
  sensor.setAccessMode(sensor.MASTERCONTROLLEDMODE);
  sensor.disableTemp();
  sensor.updateData();
  rawAngle.setup(sensor.getAzimuth() * 180 / PI);
  state.setup(rawAngle.current());

  setupCompleted = true;
}

void AngleSensor::setZero() {
  state.update(0);
}

void AngleSensor::update() {
  if (rawAngle.currentDuration() > sensor.getMeasurementDelay()) {
    sensor.updateData();
  } else {
    return;
  }
  rawAngle.update(sensor.getAzimuth() * 180 / PI);

  if (!accumulate) {
    state.update(rawAngle.current());
    return;
  }

  float delta = rawAngle.current() - rawAngle.previous();
  if (abs(delta) > overflowDeltaThreshold) {
    int deltaSign = (0.0f < delta) - (delta < 0.0f);
    delta -= deltaSign * 360.0f;
  }
  state.update(state.current() + delta);
}


} }

