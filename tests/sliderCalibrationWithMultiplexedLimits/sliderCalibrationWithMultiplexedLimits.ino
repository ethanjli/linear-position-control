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
#include <Motion/Neutral.h>
#include <LinearActuator.h>

using namespace LinearPositionControl;
using namespace Components;

// Compile-time flags

using MultiplexedDirectionCalibrator = Calibration::Direction<MultiplexedLimits>;
using MotionController = Motion::Neutral<MultiplexedLimits>;
using Actuator = LinearActuator<MultiplexedDirectionCalibrator, MotionController>;

// Singletons

Motors motors = Motors();

// Globals

Motor motor(motors, M2);
DebouncedButton leftAndRight(12, interruptCounter12, 50);
MultiplexedLimits limits(leftAndRight);
MultiplexedDirectionCalibrator directionCalibrator(motor, limits);
MotionController motionController(motor);
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
