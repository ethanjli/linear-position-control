#define NEEDFORSPEED
#define INTERRUPT_FLAG_PIN12 interruptCounter12
#define INTERRUPT_FLAG_PIN8 interruptCounter8
#include <EnableInterrupt.h>

//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#include <DebouncedButton.h>
#include <Motors.h>
#include <Limits.h>
#include <Calibration/Direction.h>
#include <Motion/Oscillator.h>
#include <LinearActuator.h>

using namespace LinearPositionControl;
using namespace Components;

// Compile-time flags

using DirectionCalibrator = Calibration::Direction<MultiplexedLimits>;
using MotionController = Motion::Oscillator<MultiplexedLimits>;
using Actuator = LinearActuator<DirectionCalibrator, MotionController>;

// Singletons

Motors motors;

// Globals

Motor motor = Motor(motors, M2);
DebouncedButton leftAndRight(12, interruptCounter12, 50);
MultiplexedLimits limits(leftAndRight);
DirectionCalibrator directionCalibrator(motor, limits);
MotionController motionController(motor, limits);
Actuator actuator(directionCalibrator, motionController);

void setup() {
#ifndef DISABLE_LOGGING
  Serial.begin(115200);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
#endif
  actuator.setup();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  actuator.update();
}
