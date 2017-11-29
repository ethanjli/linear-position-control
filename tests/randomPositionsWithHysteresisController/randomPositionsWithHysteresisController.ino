//#define DISABLE_LOGGING
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
  actuator.positionCalibrator.expectedNumEdges = 32;
  actuator.motor.speed = 127;
  randomSeed(analogRead(0));
}

void loop() {
  actuator.update();
#ifdef DISABLE_LOGGING
#ifndef DISABLE_PLOT
  Serial.print(analogRead(A1));
  Serial.print(',');
  Serial.println(1023 * digitalRead(A1));
#endif
#endif
  if (actuator.state.current() != Actuator::State::operating) return;
  if (actuator.motionController.state.current() != Actuator::MotionController::State::maintaining) return;
  if (actuator.motionController.state.currentDuration() < 2000) return;
  int newTargetPosition = actuator.positionTracker.mapPositionFrom(random(numCuvettes), numCuvettes - 1);
  actuator.motionController.targetPosition.update(newTargetPosition);
}
