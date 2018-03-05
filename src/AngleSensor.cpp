#include "AngleSensor.h"

namespace LinearPositionControl { namespace Components {

AngleSensor::AngleSensor(Tlv493d &sensor) :
  sensor(sensor)
{
}

void AngleSensor::setup() {
  if (setupCompleted) return;

  sensor.begin();
  sensor.setAccessMode(sensor.MASTERCONTROLLEDMODE);
  sensor.disableTemp();
  sensor.updateData();
  state.setup(sensor.getAzimuth() * 180 / PI);

  setupCompleted = true;
}

void AngleSensor::update() {
  if (state.currentDuration() > sensor.getMeasurementDelay()) sensor.updateData();
  state.update(sensor.getAzimuth() * 180 / PI);
}


} }

