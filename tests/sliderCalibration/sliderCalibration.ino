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
#include <Motion/Neutral.h>
#include <LinearActuator.h>

using namespace LinearPositionControl;
using namespace Components;

// Compile-time flags

using AbsoluteDirectionCalibrator = Calibration::Direction<AbsoluteLimits>;
using MotionController = Motion::Neutral<AbsoluteLimits>;
using Actuator = LinearActuator<AbsoluteDirectionCalibrator, MotionController>;

// Singletons

Motors motors;

// Globals

LED led(LED_BUILTIN);
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
  led.setup();
}

void loop() {
  actuator.update();
  led.update();
  if (actuator.state.current() == Actuator::State::calibratingDirection &&
      actuator.state.previous() != Actuator::State::calibratingDirection) {
    led.lowInterval = 50;
    led.highInterval = 50;
    led.blink();
  } else if (actuator.state.current() == Actuator::State::calibratingPosition &&
      actuator.state.previous() != Actuator::State::calibratingPosition) {
    led.lowInterval = 50;
    led.highInterval = 200;
    led.blink();
  } else if (actuator.state.current() == Actuator::State::operating &&
      actuator.state.previous() != Actuator::State::operating) {
    led.lowInterval = 50;
    led.highInterval = 800;
    led.blink();
  }
}
