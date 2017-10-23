//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#define SENSOR_PIN2
#define SENSOR_PIN12
#include <LinearPositionControl.h>
#include <OpticalSensor.h>
#include <Tracking/Discrete.h>
#include <Motion/Hysteresis.h>
using namespace LinearPositionControl;

// Singletons

SharedComponents shared;

// Globals

using Actuator = MultiplexedLinearActuator<Components::EdgeCounter, Tracking::Discrete, Motion::Hysteresis>;
Actuator actuator(shared, M2, 2, 12);

void setup() {
#ifndef DISABLE_LOGGING
  Serial.begin(115200);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
#endif
  actuator.setup();
  actuator.positionCalibrator.expectedNumEdges = 32;
  actuator.motor.speed = 63;
  randomSeed(analogRead(0));
}

void loop() {
  actuator.update();
  if (actuator.state.current() != Actuator::State::operating) return;
  if (actuator.motionController.state.current() != Actuator::MotionController::State::maintaining) return;
  if (actuator.motionController.state.currentDuration() < 1000) return;
  int newTargetPosition = map(random(0, 5), 0, 5, 0, actuator.positionTracker.getNumTotalEdges());
  actuator.motionController.targetPosition.update(newTargetPosition);
}
