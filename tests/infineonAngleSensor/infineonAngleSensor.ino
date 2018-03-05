#include <Motors.h>
#include <AngleSensor.h>

using namespace LinearPositionControl::Components;

Motors motors;
Motor motor(motors, M3);
AngleSensor angleSensor;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  motor.setup();
  angleSensor.setup();
  motor.run(200);
}

void loop() {
  angleSensor.update();

  if (angleSensor.state.justChanged()) {
    Serial.println(angleSensor.state.current());
  }

  if (motor.state.currentDuration() > 1075) {
    if (motor.state.current() != Motor::State::neutral) {
      motor.neutral();
    } else {
      motor.opposite();
    }
  }
}

