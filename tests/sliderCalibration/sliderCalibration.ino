#define NEEDFORSPEED
#define INTERRUPT_FLAG_PIN12 interruptCounter12
#define INTERRUPT_FLAG_PIN8 interruptCounter8
#include <EnableInterrupt.h>
#include <DebouncedButton.h>
#include <Motors.h>
#include <LinearActuator.h>

const bool DEBUG_SERIAL = false;

// Globals

DebouncedButton left(12, &interruptCounter12, 50);
DebouncedButton right(8, &interruptCounter8, 50);
Motors motors = Motors();
AbsoluteLinearActuator<DEBUG_SERIAL> actuator = AbsoluteLinearActuator<DEBUG_SERIAL>(&motors, M2, &left, &right);

void setup() {
  if (DEBUG_SERIAL) Serial.begin(115200);
  actuator.setup();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  actuator.update();
}
