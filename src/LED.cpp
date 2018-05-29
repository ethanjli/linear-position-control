#include "LED.h"

namespace LinearPositionControl { namespace Components {

// LED

LED::LED(uint8_t ledPin) :
  ledPin(ledPin), led(LEDFader(ledPin)) {
}

void LED::setup() {
  if (setupCompleted) return;

  pinMode(ledPin, OUTPUT);
  led.set_value(0);
  state.setup(State::off);

  setupCompleted = true;
}

void LED::update() {
  if (state.current() == State::off || state.current() == State::on) return; // nothing to update!
  if (periods == 0) {
    onPeriodsFinished();
    return;
  }
  if (state.current() == State::fadingHigh || state.current() == State::fadingLow) {
    updateFading();
  } else {
    updateBlinking();
  }
}

void LED::off() {
  led.set_value(0);
  state.update(State::off);
}

void LED::on() {
  led.set_value(highBrightness);
  state.update(State::on);
}

void LED::on(uint8_t brightness) {
  led.set_value(brightness);
  state.update(State::on);
}

void LED::blink() {
  if (periods == 0) {
    led.set_value(lowBrightness);
    state.update(State::off);
  } else {
    led.set_value(highBrightness);
    state.update(State::blinkingHigh);
  }
}

void LED::fade() {
  led.set_value(lowBrightness);
  led.fade(highBrightness, highInterval);
  state.update(State::fadingHigh);
}

void LED::onPeriodsFinished() {
  led.set_value(lowBrightness);
  state.update(State::off);
}

void LED::updateFading() {
  led.update();
  if (led.is_fading()) return;
  if (state.current() == State::fadingLow) {
    led.fade(highBrightness, highInterval);
    if (periods > 0) --periods;
  } else { // state.current() == State::fadingHigh
    led.fade(lowBrightness, lowInterval);
  }
}

void LED::updateBlinking() {
  if (state.current() == State::blinkingHigh && state.currentDuration() < highInterval) return;
  if (state.current() == State::blinkingLow && state.currentDuration() < lowInterval) return;
  if (state.current() == State::blinkingLow) {
    led.set_value(highBrightness);
    state.update(State::blinkingHigh);
    if (periods > 0) --periods;
  } else { // state.current() == State::blinkingHigh
    led.set_value(lowBrightness);
    state.update(State::blinkingLow);
  }
}

// SimpleLED

SimpleLED::SimpleLED(uint8_t ledPin) :
  ledPin(ledPin) {
}

void SimpleLED::setup() {
  if (setupCompleted) return;

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  state.setup(State::off);

  setupCompleted = true;
}

void SimpleLED::update() {
  if (state.current() == State::off || state.current() == State::on) return; // nothing to update!
  if (periods == 0) {
    off();
    return;
  }
  updateBlinking();
}

void SimpleLED::off() {
  digitalWrite(ledPin, LOW);
  state.update(State::off);
}

void SimpleLED::on() {
  digitalWrite(ledPin, HIGH);
  state.update(State::on);
}

void SimpleLED::blink() {
  if (periods == 0) {
    off();
  } else {
    digitalWrite(ledPin, HIGH);
    state.update(State::blinkingHigh);
  }
}

void SimpleLED::updateBlinking() {
  if (state.current() == State::blinkingHigh && state.currentDuration() < highInterval) return;
  if (state.current() == State::blinkingLow && state.currentDuration() < lowInterval) return;
  if (state.current() == State::blinkingLow) {
    digitalWrite(ledPin, HIGH);
    state.update(State::blinkingHigh);
    if (periods > 0) --periods;
  } else { // state.current() == State::blinkingHigh
    digitalWrite(ledPin, LOW);
    state.update(State::blinkingLow);
  }
}

} }

