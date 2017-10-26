#define DISABLE_LOGGING
#include <ArduinoLog.h>

#include <elapsedMillis.h>

#define SENSOR_PIN4
#define SENSOR_PIN12
#include <LinearPositionControl.h>
#include <OpticalSensor.h>
#include <Tracking/Discrete.h>
#include <Motion/Hysteresis.h>
#include <AnalogSensor.h>
using namespace LinearPositionControl;

// Singletons

SharedComponents shared;

// Hardware Abstraction Layer

using Actuator = MultiplexedLinearActuator<Components::EdgeCounter, Tracking::Discrete, Motion::Hysteresis>;
Actuator actuator(shared, M2, 4, 12);
Components::AnalogSensor potentiometer(A0);
Components::AnalogSensor opticalSensor(A1);

// Ground Truth Position Parameters

const int numPositions = 1024;
int targetPosition = -1;
elapsedMillis targetingTimer;
elapsedMicros targetingTimerMicroseconds;
bool maintainedPositionReported = false;

inline int mapToEdgeCount(int position) {
  return actuator.positionTracker.mapPositionFrom(position, 0, numPositions - 1);
}

inline int mapToPosition(int edgeCount) {
  return actuator.positionTracker.mapPositionTo(edgeCount, 0, numPositions - 1);
}

void printHeader() {
  // Clock
  Serial.print(F("targetingTimeMilliseconds,"));
  Serial.print(F("targetingTimeMicroseconds,"));
  // Target
  Serial.print(F("targetPosition,"));
  Serial.print(F("targetEdgesFromLeft,"));
  // Ground truth
  Serial.print(F("groundTruthPosition,"));
  // Baseline prediction
  Serial.print(F("estimatedPosition,"));
  // Estimated position features
  Serial.print(F("estimatedEdgesFromLeft,"));
  // Optical sensor features
  Serial.print(F("opticalSensorDarkness,"));
  // Motor features
  Serial.print(F("motorDirection,"));
  Serial.print(F("motorDuty"));
  
  Serial.println();
}

void reportState() {
  if (targetPosition == -1) return;
  
  // Clock
  Serial.print(targetingTimer);
  Serial.print(',');
  Serial.print(targetingTimerMicroseconds);
  Serial.print(',');
  // Target
  Serial.print(targetPosition);
  Serial.print(',');
  Serial.print(mapToEdgeCount(targetPosition));
  Serial.print(',');
  // Ground truth
  Serial.print(potentiometer.state.current());
  Serial.print(',');
  // Baseline prediction
  Serial.print(mapToPosition(actuator.positionTracker.position.current()));
  Serial.print(',');
  // Estimated position features
  Serial.print(actuator.positionTracker.position.current());
  Serial.print(',');
  // Optical sensor features
  Serial.print(opticalSensor.state.current());
  Serial.print(',');
  // Motor features
  switch (actuator.motor.state.current()) {
    case Components::States::Motor::forwards:
      Serial.print(1);
      break;
    case Components::States::Motor::backwards:
      Serial.print(-1);
      break;
    default:
      Serial.print(0);
  }
  Serial.print(',');
  Serial.print(actuator.motor.speed);
  
  Serial.println();
}

void setNewTargetPosition() {
  targetPosition = random(numPositions);
  actuator.motionController.targetPosition.update(mapToEdgeCount(targetPosition));
  targetingTimer = 0;
  targetingTimerMicroseconds = 0;
  maintainedPositionReported = false;
}

void setup() {
  Serial.begin(115200);
  actuator.setup();
  actuator.positionCalibrator.expectedNumEdges = 24;
  actuator.motor.speed = 255;
  potentiometer.setup();
  opticalSensor.setup();
  randomSeed(analogRead(0));
  printHeader();
}

void loop() {
  actuator.update();
  potentiometer.update();
  opticalSensor.update();
  if (actuator.state.current() != Actuator::State::operating) return;
  if (actuator.motionController.state.current() != Actuator::MotionController::State::maintaining) {
    reportState();
    return;
  }
  if (!maintainedPositionReported && actuator.motor.state.current() == Components::States::Motor::braking) {
    reportState();
    maintainedPositionReported = true;
  }
  if (actuator.motionController.state.currentDuration() < 1000) return;
  reportState();
  setNewTargetPosition();
}
