//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#define SENSOR_PIN4
#define SENSOR_PIN12
#include <LinearPositionControl.h>
#include <OpticalSensor.h>
#include <Tracking/Discrete.h>
#include <Motion/Hysteresis.h>
#include <AnalogSensor.h>
using namespace LinearPositionControl;

const int numPositions = 1024;

// Singletons

SharedComponents shared;

// Globals

using Actuator = MultiplexedLinearActuator<Components::EdgeCounter, Tracking::Discrete, Motion::Hysteresis>;
Actuator actuator(shared, M2, 4, 12);
Components::AnalogSensor potentiometer(A0);

void setup() {
#ifndef DISABLE_LOGGING
  Serial.begin(115200);
  Log.begin(LOG_LEVEL_NOTICE, &Serial);
#endif
  actuator.setup();
  actuator.positionCalibrator.expectedNumEdges = 24;
  actuator.motor.speed = 255;
  potentiometer.setup();
  randomSeed(analogRead(0));
}

void loop() {
  actuator.update();
  potentiometer.update();
  if (actuator.state.current() != Actuator::State::operating) return;
  if (actuator.motionController.state.current() != Actuator::MotionController::State::maintaining) return;
  if (actuator.motionController.state.currentDuration() < 1000) return;
  Log.notice(F("Actual tracked position is %d." CR), actuator.positionTracker.position.current());
  Log.notice(F("Ground truth position is %d, which maps to %d." CR), potentiometer.state.current(), actuator.positionTracker.mapPositionFrom(potentiometer.state.current(), 0, numPositions - 1));
  int newTargetPosition = random(numPositions);
  Log.notice(F("Targeting position %d, which maps to %d." CR), newTargetPosition, actuator.positionTracker.mapPositionFrom(newTargetPosition, 0, numPositions - 1));
  actuator.motionController.targetPosition.update(actuator.positionTracker.mapPositionFrom(newTargetPosition, 0, numPositions - 1));
}
