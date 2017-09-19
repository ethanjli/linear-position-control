/*
 *  This example shows how to combine the EnableInterrupt
 *  library with some simple debouncing code for reading
 *  button presses consistently.
 *  
 *  The interrupt is attached to digital input 5 and the
 *  service routine just toggles the onboard LED status.
 *  
 *  Tested on: ATmega328P
 *  
 *  Example by Lorenzo Cafaro <lorenzo@ibisco.net>
 *
*/

// Interrupts

#define NEEDFORSPEED

#define INTERRUPT_FLAG_PIN12 interruptCounter12
#define INTERRUPT_FLAG_PIN8 interruptCounter8

#include <EnableInterrupt.h>

// Limit switches

#include <elapsedMillis.h>

class DebouncedButton {
  public:
  DebouncedButton(int buttonPin, volatile uint8_t *interruptCounter, unsigned int debounceDelay) :
    buttonPin(buttonPin), interruptCounter(interruptCounter), debounceDelay(debounceDelay) {}

  void setup() {
    pinMode(buttonPin, INPUT_PULLUP);
    enableInterruptFast(buttonPin, RISING);
    buttonState = digitalRead(buttonPin);
    eventStatePressed = buttonState;
    eventStateReleased = !buttonState;
  }

  void update() {
    if (*interruptCounter) {
      *interruptCounter = 0;
      debounceTimer = 0;
      buttonChanged = true;
    }
    if (buttonChanged && debounceTimer > debounceDelay) {
      buttonState = digitalRead(buttonPin);
      eventStateChanged = true;
      eventStatePressed = eventStateChanged && buttonState;
      eventStateReleased = eventStateChanged && !buttonState;
      buttonChanged = false;
    }
  }

  bool state() {
    return buttonState;
  }

  // Event flags
  bool eventStateChanged = false; // set to true when state changes, must be reset by client
  bool eventStatePressed = false; // set to true when button is pressed, must be reset by client
  bool eventStateReleased = false; // set to true when button is released, must be reset by client

  private:
  int buttonPin;
  volatile uint8_t *interruptCounter;
  unsigned int debounceDelay;
  elapsedMillis debounceTimer;
  bool buttonChanged = false;
  bool buttonState;
};

// Motors
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

// Globals

DebouncedButton left(12, &interruptCounter12, 50);
DebouncedButton right(8, &interruptCounter8, 50);
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motor = AFMS.getMotor(2);

void setup() {
  left.setup();
  right.setup();
  AFMS.begin();
  delay(2000);
  motor->setSpeed(64);
  motor->run(FORWARD);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  left.update();
  right.update();
  if (left.eventStatePressed) {
    motor->run(RELEASE);
    delay(1000);
    motor->run(FORWARD);
    left.eventStatePressed = false;
  }
  if (right.eventStatePressed) {
    motor->run(RELEASE);
    delay(1000);
    motor->run(BACKWARD);
    right.eventStatePressed = false;
  }
}
