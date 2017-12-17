//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#include <AnalogSensor.h>
#include <Motors.h>
#include <PIDControl.h>
#include <SerialIO.h>

using namespace LinearPositionControl;

// Hardware Parameters

const uint8_t potentiometerPin = A0;
const MotorPort motorPort = M2;

const int minPosition = 11;
const int maxPosition = 999;

// Controller Parameters

const double pidKp = 8;
const double pidKd = 0.1; // friction compensation
const double pidKi = 0.1; // improved accuracy
const int pidSampleTime = 20;

const int feedforward = 7; // gravity compensation
const int brakeThreshold = 80; // prevent high-pitch motor whine when the motor is stuck due to friction anyways

const unsigned long completionDelay = 100; // delay after motor stops moving before declaring completion of movement

// Singletons

Components::Motors motors;

// Globals

Components::AnalogSensor potentiometer(potentiometerPin);
DiscretePID pid(
  potentiometer.state, pidKp, pidKd, pidKi,
  -255 - feedforward, 255 - feedforward, pidSampleTime,
  minPosition, maxPosition
);
Components::MotorSpeedAdjuster speedAdjuster(pid.output, feedforward, brakeThreshold);
Components::Motor motor(motors, motorPort);
IntParser setpointParser;
bool reportedCompletion = false;

void setup() {
  Serial.begin(115200);
#ifndef DISABLE_LOGGING
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
#endif
  motors.setup();
  motor.swapDirections();
  potentiometer.setup();
  pid.setup();
  setpointParser.setup();
  waitForSerialHandshake();
}

void loop() {
  potentiometer.update();
  pid.update();
  speedAdjuster.update();
  motor.run(speedAdjuster.output.current());
  setpointParser.update();
  pid.setSetpoint(setpointParser.result.current());
  if (setpointParser.justUpdated) {
    reportedCompletion = false;
    setpointParser.justUpdated = false;
  }
  if (pid.setpoint.settled(completionDelay) && speedAdjuster.output.settledAt(0, completionDelay) && !reportedCompletion) {
    Serial.print('[');
    Serial.print(pid.getInput());
    Serial.println(']');
    reportedCompletion = true;
  }
}
