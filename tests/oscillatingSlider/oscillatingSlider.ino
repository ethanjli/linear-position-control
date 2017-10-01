#define NEEDFORSPEED
#define INTERRUPT_FLAG_PIN12 interruptCounter12
#define INTERRUPT_FLAG_PIN8 interruptCounter8
#include <EnableInterrupt.h>

//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#include <LED.h>
#include <DebouncedButton.h>
#include <Motors.h>
#include <Limits.h>
#include <Calibration/Direction.h>
#include <Motion/Oscillator.h>
#include <LinearActuator.h>

using namespace LinearPositionControl;
using namespace Components;

// Compile-time flags

using AbsoluteDirectionCalibrator = Calibration::Direction<AbsoluteLimits>;
using MotionController = Motion::Oscillator<AbsoluteLimits>;
using Actuator = LinearActuator<AbsoluteDirectionCalibrator, MotionController>;

// Singletons

Motors motors;

// Globals

LED led(LED_BUILTIN);
Motor motor(motors, M2);
DebouncedButton right(12, interruptCounter12, 5);
DebouncedButton left(8, interruptCounter8, 5);
AbsoluteLimits limits(left, right);
AbsoluteDirectionCalibrator directionCalibrator(motor, limits);
MotionController motionControl(motor, limits);
Actuator actuator(directionCalibrator, motionControl);

void setup() {
#ifndef DISABLE_LOGGING
  Serial.begin(115200);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
#endif
  actuator.setup();
  led.setup();
}

void loop() {
  actuator.update();
  led.update();
  if (actuator.state.justEntered(Actuator::State::calibratingDirection)) {
    led.lowInterval = 50;
    led.highInterval = 50;
    led.blink();
  } else if (actuator.state.justEntered(Actuator::State::calibratingPosition)) {
    led.lowInterval = 50;
    led.highInterval = 200;
    led.blink();
  } else if (actuator.state.justEntered(Actuator::State::operating)) {
    led.lowInterval = 50;
    led.highInterval = 800;
    led.blink();
  }
}
