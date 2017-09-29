#define NEEDFORSPEED
#define INTERRUPT_FLAG_PIN12 interruptCounter12
#define INTERRUPT_FLAG_PIN8 interruptCounter8
#include <EnableInterrupt.h>
#include <DebouncedButton.h>
#include <Motors.h>
#include <Limits.h>
#include <DirectionCalibration.h>
#include <LinearActuator.h>

// Compile-time flags

const bool DEBUG_SERIAL = false;
using DirectionCalibrator = AbsoluteDirectionCalibrator<DEBUG_SERIAL>;

// Singletons

Motors motors = Motors();

// Globals

Motor motor(motors, M2);
DebouncedButton right(12, interruptCounter12, 50);
DebouncedButton left(8, interruptCounter8, 50);
AbsoluteLimits limits(left, right);
DirectionCalibrator directionCalibrator(motor, limits);
LinearActuator<DirectionCalibrator> actuator(directionCalibrator);

void setup() {
  if (DEBUG_SERIAL) Serial.begin(115200);
  actuator.setup();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  actuator.update();
}
