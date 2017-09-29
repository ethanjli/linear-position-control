#define NEEDFORSPEED
#define INTERRUPT_FLAG_PIN12 interruptCounter12
#define INTERRUPT_FLAG_PIN8 interruptCounter8
#include <EnableInterrupt.h>
#include <DebouncedButton.h>
#include <Motors.h>
#include <Limits.h>
#include <DirectionCalibration.h>
#include <Motion/Neutral.h>
#include <LinearActuator.h>

// Compile-time flags

const bool DEBUG_SERIAL = true;
using DirectionCalibrator = MultiplexedDirectionCalibrator<DEBUG_SERIAL>;
using MotionController = Neutral<DEBUG_SERIAL>;
using Actuator = LinearActuator<DirectionCalibrator, MotionController>;

// Singletons

Motors motors = Motors();

// Globals

Motor motor(motors, M2);
DebouncedButton leftAndRight(12, interruptCounter12, 50);
MultiplexedLimits limits(leftAndRight);
DirectionCalibrator directionCalibrator(motor, limits);
MotionController motionController(motor);
Actuator actuator(directionCalibrator, motionController);

void setup() {
  if (DEBUG_SERIAL) Serial.begin(115200);
  actuator.setup();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  actuator.update();
}
