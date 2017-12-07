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
CalibrationRig<Actuator> calibrationRig(shared, actuator, A0);

const int minSpeed = 120;
const int maxSpeed = 240;
const int numSpeeds = 12;
const int speedIncrement = (maxSpeed - minSpeed) / numSpeeds;

void setup() {
  calibrationRig.setup();
  actuator.positionCalibrator.expectedNumEdges = 40;
  actuator.motor.speed = 150;
  //randomSeed(analogRead(0));
  randomSeed(0); // traindev
  //randomSeed(1); // test
  calibrationRig.waitForSerialHandshake();
  calibrationRig.printHeader();
}

void loop() {
  calibrationRig.update();
  if (actuator.state.current() != Actuator::State::operating) return;
  if (calibrationRig.episodeID > 0 && calibrationRig.targeting) calibrationRig.printState();
  if (actuator.motionController.state.current() != Actuator::MotionController::State::maintaining) return;
  if (actuator.motionController.state.currentDuration() < 250) return;
  if (calibrationRig.targeting) {
    calibrationRig.startPreEpisodeLocalization();
  } else {
    //actuator.motor.speed = minSpeed + speedIncrement * random(numSpeeds);
    calibrationRig.startNewEpisode();
  }
}
