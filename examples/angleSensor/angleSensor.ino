#define LPC_Components_Motors
#define LPC_Components_AngleSensor
#include <LinearPositionControl.h>

using namespace LinearPositionControl::Components;

Motors motors;
Motor motor(motors, M3);
AngleSensor angleSensor(1);
elapsedMillis timer;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  motor.setup();
  angleSensor.setup();
  motor.run(200);
  timer = 0;
}

void loop() {
  angleSensor.update();

  if (angleSensor.state.justChanged()) {
    Serial.println(angleSensor.state.current);
  }

  if (timer > 1000) {
    timer = 0;
    if (motor.speed != 0) {
      motor.neutral();
    } else {
      motor.swapDirections();
      motor.forwards(100);
    }
  }
}

