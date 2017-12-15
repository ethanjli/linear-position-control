//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#include <AnalogSensor.h>
#include <Motors.h>
#include <PIDControl.h>

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

// Singletons

Components::Motors motors;

// Globals

Components::AnalogSensor potentiometer(potentiometerPin);
DiscretePID pid(
  potentiometer.state, pidKp, pidKd, pidKi,
  -255 - feedforward, 255 - feedforward, pidSampleTime
);
Components::Motor motor(motors, motorPort);

void setup() {
  Serial.begin(115200);
#ifndef DISABLE_LOGGING
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
#endif
  motors.setup();
  motor.swapDirections();
  potentiometer.setup();
  pid.setup();
}

void loop() {
  potentiometer.update();
  pid.update();
  int motorSpeed = feedforward + pid.output.current();
  if (abs(motorSpeed) < brakeThreshold) motorSpeed = 0;
  motor.run(motorSpeed);
  Serial.println(pid.getInput());
}
