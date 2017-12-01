//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#define SENSOR_PIN4
#define SENSOR_PIN12
#include <LinearPositionControl.h>
#include <OpticalSensor.h>
#include <Tracking/Position/Discrete.h>
#include <Motion/Hysteresis.h>
#include <AnalogSensor.h>
using namespace LinearPositionControl;

// Singletons

SharedComponents shared;

// Hardware Abstraction Layer

using Actuator = MultiplexedLinearActuator<Components::EdgeCounter, Tracking::Position::Discrete, Motion::Hysteresis>;
Actuator actuator(shared, M2, 4, 12);
CalibrationRig<Actuator> calibrationRig(shared, actuator, A0, A1);

void setup() {
  calibrationRig.setup();
  actuator.positionCalibrator.expectedNumEdges = 40;
  actuator.motor.speed = 255;
  randomSeed(analogRead(0));
  calibrationRig.printHeader();
}

void loop() {
  calibrationRig.update();
  if (actuator.state.current() != Actuator::State::operating) return;
#ifdef DISABLE_LOGGING
  if (calibrationRig.targetID > 0 && calibrationRig.targeting) calibrationRig.printState();
#endif
  if (actuator.motionController.state.current() != Actuator::MotionController::State::maintaining) return;
  if (actuator.motionController.state.currentDuration() < 250) return;
  if (calibrationRig.targeting) {
    calibrationRig.setLocalizationPosition();
  } else {
    calibrationRig.setNewTargetPosition();
  }
}
