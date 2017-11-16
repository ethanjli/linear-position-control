//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#define SENSOR_PIN4
#define SENSOR_PIN12
#include <LinearPositionControl.h>
#include <OpticalSensor.h>
#include <Tracking/Position/Discrete.h>
#include <Motion/Hysteresis.h>
using namespace LinearPositionControl;

const int numCuvettes = 6;
const int cuvetteOrder[] = {0, 4, 1, 3, 2, 5};
int cuvetteIndex = 0;

// Singletons

SharedComponents shared;

// Globals

using Actuator = MultiplexedLinearActuator<Components::EdgeCounter, Tracking::Position::Discrete, Motion::Hysteresis>;
Actuator actuator(shared, M2, 4, 12);

void setup() {
#ifndef DISABLE_LOGGING
  Serial.begin(115200);
  Log.begin(LOG_LEVEL_NOTICE, &Serial);
#endif
  actuator.setup();
  actuator.positionCalibrator.expectedNumEdges = 32;
  actuator.motor.speed = 80;
  randomSeed(analogRead(0));
}

void loop() {
  actuator.update();
  if (actuator.state.current() != Actuator::State::operating) return;
  if (actuator.motionController.state.current() != Actuator::MotionController::State::maintaining) return;
  if (actuator.motionController.state.currentDuration() < 1000) return;
  int newTargetPosition = actuator.positionTracker.mapPositionFrom(cuvetteOrder[cuvetteIndex], numCuvettes - 1);
  cuvetteIndex = (cuvetteIndex + 1) % numCuvettes;
  actuator.motionController.targetPosition.update(newTargetPosition);
}
