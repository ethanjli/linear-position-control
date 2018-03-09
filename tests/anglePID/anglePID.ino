#include <Motors.h>
#include <AngleSensor.h>
#include <PIDControl.h>
#include <SerialIO.h>

using namespace LinearPositionControl;

const int minAngle = 0 - 110;
const int maxAngle = 360 * 2 - 110;

const double pidKp = 8;
const double pidKd = 0;
const double pidKi = 0;
const int pidSampleTime = 20;

const unsigned long completionDelay = 100; // delay after motor stops moving before declaring completion of movement

// Singletons

Components::Motors motors;

// Globals

Components::Motor motor(motors, M3);
Components::AngleSensor angleSensor;
ContinuousPID pid(
  angleSensor.state, pidKp, pidKd, pidKi,
  -255, 255, pidSampleTime,
  minAngle, maxAngle
);
IntParser setpointParser;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  motor.setup();
  angleSensor.setup();
  pid.setup();
  setpointParser.setup();
  waitForSerialHandshake();
}

void loop() {
  angleSensor.update();
  pid.update();
  motor.run(pid.output.current());
  setpointParser.update();
  pid.setSetpoint(setpointParser.result.current());
  Serial.print('[');
  Serial.print(pid.getInput());
  Serial.println(']');
}

