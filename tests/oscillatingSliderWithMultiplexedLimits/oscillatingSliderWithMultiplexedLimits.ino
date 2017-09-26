#define NEEDFORSPEED
#define INTERRUPT_FLAG_PIN12 interruptCounter12
#define INTERRUPT_FLAG_PIN8 interruptCounter8
#include <EnableInterrupt.h>
#include <DebouncedButton.h>
#include <Motors.h>
#include <LinearActuator.h>

// Globals

DebouncedButton limits(12, &interruptCounter12, 50);
Motors motors = Motors();
AbsoluteLinearActuator actuator = MultiplexedLinearActuator(&motors, M2, &limits);

void setup() {
  Serial.begin(115200);
  actuator.setup();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  actuator.update();
}
