#define NEEDFORSPEED
#define INTERRUPT_FLAG_PIN12 interruptCounter12
#define INTERRUPT_FLAG_PIN8 interruptCounter8
#define INTERRUPT_FLAG_PIN2 interruptCounter2
#include <EnableInterrupt.h>

//#define DISABLE_LOGGING
#include <ArduinoLog.h>

#include <LED.h>
#include <DebouncedButton.h>
#include <Motors.h>
#include <OpticalSensor.h>
#include <Limits.h>
#include <Calibration/Direction.h>
#include <Calibration/Position.h>
#include <Motion/Oscillator.h>
#include <LinearActuator.h>

using namespace LinearPositionControl;
using namespace Components;

// Compile-time flags

using MultiplexedDirectionCalibrator = Calibration::Direction<MultiplexedLimits>;
using PositionCalibrator = Calibration::Position<MultiplexedLimits, EdgeCounter>;
using MotionController = Motion::Oscillator<MultiplexedLimits>;
using Actuator = LinearActuator<MultiplexedDirectionCalibrator, PositionCalibrator, MotionController>;

// Singletons

Motors motors;

// Globals

LED led(LED_BUILTIN);
Motor motor = Motor(motors, M2);
EdgeCounter edgeCounter(2, interruptCounter2);
DebouncedButton leftAndRight(12, interruptCounter12, 5);
MultiplexedLimits limits(leftAndRight);
MultiplexedDirectionCalibrator directionCalibrator(motor, limits);
PositionCalibrator positionCalibrator(motor, limits, edgeCounter);
MotionController motionController(motor, limits);
Actuator actuator(directionCalibrator, positionCalibrator, motionController);

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
