#define DISABLE_LOGGING
//#define DISABLE_PLOT
#include <ArduinoLog.h>

#define SENSOR_PIN4
#define SENSOR_PIN12
#include <LinearPositionControl.h>
#include <OpticalSensor.h>
#include <Tracking/Position/Discrete.h>
#include <Motion/Hysteresis.h>
using namespace LinearPositionControl;

const int numCuvettes = 6;

// Singletons

SharedComponents shared;

// Globals

using Actuator = MultiplexedLinearActuator<Components::EdgeCounter, Tracking::Position::Discrete, Motion::Hysteresis>;
Actuator actuator(shared, M2, 4, 12);

void setup() {
#ifndef DISABLE_LOGGING
  Serial.begin(115200);
  Log.begin(LOG_LEVEL_NOTICE, &Serial);
#else
#ifndef DISABLE_PLOT
  Serial.begin(115200);
#endif  
#endif
  actuator.setup();
  actuator.positionCalibrator.expectedNumEdges = 40;
  actuator.motor.speed = 255;
  randomSeed(analogRead(0));
}

void loop() {
  actuator.update();
#ifdef DISABLE_LOGGING
#ifndef DISABLE_PLOT
  //if (actuator.positionTracker.position.justChanged()) {
    Serial.print("0,40,");
    Serial.println(actuator.positionTracker.position.current());
  //}
#endif
#endif
  if (actuator.state.current() != Actuator::State::operating) return;
  if (actuator.motionController.state.current() != Actuator::MotionController::State::maintaining) return;
  if (actuator.motionController.state.currentDuration() < 250) return;
  int newTargetPosition = actuator.positionTracker.mapPositionFrom(random(numCuvettes), numCuvettes - 1);
  actuator.motionController.targetPosition.update(newTargetPosition);
}
