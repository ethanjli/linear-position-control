//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#include <AbsoluteLinearPositionControl.h>
#include <SerialIO.h>

using namespace LinearPositionControl;

// Hardware Parameters

const unsigned long completionDelay = 100; // delay after motor stops moving before declaring completion of movement

// Singletons

Components::Motors motors;

// Globals

AbsoluteLinearActuator actuator(
  motors, M1, A0, 11, 999,
  30, 1, 0.1, 10,
  false, 0, -180, 180, -255, 255
);

IntParser setpointParser;
bool reportedCompletion = false;

void setup() {
  Serial.begin(115200);
#ifndef DISABLE_LOGGING
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
#endif
  actuator.setup();
  setpointParser.setup();
  waitForSerialHandshake();
}

void loop() {
  actuator.update();
  setpointParser.update();
  actuator.pid.setSetpoint(setpointParser.result.current);
  if (setpointParser.justUpdated) {
    reportedCompletion = false;
    setpointParser.justUpdated = false;
  }
  if (actuator.pid.setpoint.settled(completionDelay) && actuator.speedAdjuster.output.settledAt(0, completionDelay) && !reportedCompletion) {
    Serial.print('[');
    Serial.print(actuator.pid.getInput());
    Serial.println(']');
    reportedCompletion = true;
  }
}
