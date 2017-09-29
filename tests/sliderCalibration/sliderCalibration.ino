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

// Compile-time flags

using AbsoluteDirectionCalibrator = DirectionCalibrator<AbsoluteLimits>;
using MotionController = Neutral<AbsoluteLimits>;
using Actuator = LinearActuator<AbsoluteDirectionCalibrator, MotionController>;

// Singletons

Motors motors = Motors();

// Globals

Motor motor(motors, M2);
DebouncedButton right(12, interruptCounter12, 50);
DebouncedButton left(8, interruptCounter8, 50);
AbsoluteLimits limits(left, right);
AbsoluteDirectionCalibrator directionCalibrator(motor, limits);
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
