//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#include <PID_v1.h>

#include <AnalogSensor.h>
#include <LinearPositionControl.h>

using namespace LinearPositionControl;

// Parameters

const uint8_t potentiometerPin = A0;
const uint8_t motorPort = M2;

const int minPosition = 11;
const int maxPosition = 999;

const double pidKp = 8;
const double pidKd = 0.1;
const double pidKi = 0.1;
const int feedforward = 0;
const int pidSampleTime = 20;

// Singletons

SharedComponents shared;

// Globals

double pidSetpoint = 900;
double pidInput, pidOutput;

PID pid(&pidInput, &pidOutput, &pidSetpoint, pidKp, pidKd, pidKi, P_ON_E, REVERSE);

Components::AnalogSensor potentiometer(potentiometerPin);
Components::Motor motor(shared.motors, motorPort);

void setup() {
  Serial.begin(115200);
#ifndef DISABLE_LOGGING
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
#endif
  shared.setup();
  potentiometer.setup();
  pidInput = potentiometer.state.current();
  pid.SetMode(AUTOMATIC);
  pid.SetOutputLimits(-255 - feedforward, 255 - feedforward);
  pid.SetSampleTime(pidSampleTime);
}

void loop() {
  potentiometer.update();
  pidInput = potentiometer.state.current();
  pid.Compute();
  motor.run(feedforward + (int) pidOutput);
  Serial.print(minPosition);
  Serial.print(",");
  Serial.print(maxPosition);
  Serial.print(",");
  Serial.print(pidSetpoint);
  Serial.print(",");
  Serial.print(potentiometer.state.current());
  Serial.println();
}
