#define NEEDFORSPEED
#define INTERRUPT_FLAG_PIN12 interruptCounter12
#define INTERRUPT_FLAG_PIN8 interruptCounter8
#include <EnableInterrupt.h>
#include <DebouncedButton.h>
#include <Motors.h>
#include <Limits.h>
#include <LinearActuator.h>

// Compile-time flags

const bool DEBUG_SERIAL = false;

// Singletons

Motors motors = Motors();

// Globals

Motor motor = Motor(&motors, M2);
DebouncedButton left(12, &interruptCounter12, 50);
DebouncedButton right(8, &interruptCounter8, 50);
AbsoluteLimits limits = AbsoluteLimits(&left, &right);
AbsoluteLinearActuator<DEBUG_SERIAL> actuator = AbsoluteLinearActuator<DEBUG_SERIAL>(&motor, &limits);

void setup() {
  if (DEBUG_SERIAL) Serial.begin(115200);
  actuator.setup();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  actuator.update();
}
