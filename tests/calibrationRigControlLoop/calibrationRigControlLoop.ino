//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#define SENSOR_PIN4
#define SENSOR_PIN8
#define SENSOR_PIN12
#include <LinearPositionControl.h>
#include <OpticalSensor.h>
#include <Tracking/Position/Discrete.h>
#include <Motion/SerialControl.h>
#include <AnalogSensor.h>
using namespace LinearPositionControl;

// Singletons

SharedComponents shared;

// Hardware Abstraction Layer

using Actuator = UnmultiplexedLinearActuator<Components::EdgeCounter, Tracking::Position::Discrete, Motion::SerialControl>;
Actuator actuator(shared, M2, 4, 8, 12);
CalibrationRig<Actuator> calibrationRig(shared, actuator, A0);

void setup() {
  calibrationRig.setup();
  actuator.positionCalibrator.expectedNumEdges = 40;
  calibrationRig.waitForSerialHandshake();
  calibrationRig.printHeader();
}

void loop() {
  calibrationRig.update();
  if (actuator.state.current() == Actuator::State::operating) {
    calibrationRig.printState(false);
    actuator.motionController.updateFromSerial();
  }
}
