#define NEEDFORSPEED
#define INTERRUPT_FLAG_PIN12 interruptCounter12
#include <EnableInterrupt.h>
#include <DebouncedButton.h>

// Globals

DebouncedButton debouncedButton(12, &interruptCounter12, 50);
bool ledState = LOW;

void setup() {
  debouncedButton.setup();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, ledState);
}

void loop() {
  debouncedButton.update();
  if (debouncedButton.eventStatePressed) {
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
    debouncedButton.eventStatePressed = false;
  }
}
