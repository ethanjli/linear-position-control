//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#include <elapsedMillis.h>

#define LPC_Control_AbsoluteLinearPosition
#include <LinearPositionControl.h>

using namespace LinearPositionControl;

// Hardware Parameters

elapsedMillis timer;
const unsigned int repositioningInterval = 2000;

const unsigned long completionDelay = 100; // delay after motor stops moving before declaring completion of movement

// Singletons

Components::Motors motors;

// Globals

Control::AbsoluteLinearActuator actuator(
  motors, M1, A0, 11, 999,
  30, 1, 0.1, 10,
  false, 0, -180, 180, -255, 255
);

bool reportedCompletion = false;

void setup() {
  Serial.begin(115200);
#ifndef DISABLE_LOGGING
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
#endif
  actuator.setup();
  timer = 0;
}

void loop() {
  actuator.update();
  if (timer > repositioningInterval) {
    actuator.pid.setSetpoint(random(actuator.pid.getMinInput(), actuator.pid.getMaxInput()));
    reportedCompletion = false;
    timer = 0;
    Serial.print('<');
    Serial.print('p');
    Serial.print('f');
    Serial.print('>');
    Serial.print('(');
    Serial.print(actuator.pid.setpoint.current);
    Serial.println(')');
  }
  if (actuator.pid.setpoint.settled(completionDelay) && actuator.speedAdjuster.output.settledAt(0, completionDelay) && !reportedCompletion) {
    Serial.print('<');
    Serial.print('p');
    Serial.print('p');
    Serial.print('>');
    Serial.print('(');
    Serial.print(actuator.position.current);
    Serial.println(')');
    reportedCompletion = true;
  }
}
