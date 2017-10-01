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
  if (periods == 0) return; // we've finished our blink/fade cycles!
  if (state.current() == State::fadingHigh || state.current() == State::fadingLow) {
    led.update();
    if (!led.is_fading()) {
      if (state.current() == State::fadingLow) led.fade(highBrightness, highInterval);
      else led.fade(lowBrightness, lowInterval); // state.current() == State::fadingHigh
    }
    return;
  }
  // state.current() == State::blinkingHigh || state.current() == State::blinkingLow
  if ((state.current() == State::blinkingHigh && state.currentDuration() > highInterval) ||
      (state.current() == State::blinkingLow && state.currentDuration() > lowInterval)) {
    if (state.current() == State::blinkingLow) {
      led.set_value(highBrightness);
      state.update(State::blinkingHigh);
    } else { // state.current() == State::blinkingHigh
      led.set_value(lowBrightness);
      state.update(State::blinkingLow);
    }
  }
  if (periods > 0) --periods;
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
  led.set_value(lowBrightness);
  state.update(State::blinkingLow);
}

void LED::fade() {
  led.set_value(lowBrightness);
  led.fade(highBrightness, highInterval);
  state.update(State::fadingHigh);
}

} }

