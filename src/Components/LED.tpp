#ifndef LPC_Components_LED_tpp
#define LPC_Components_LED_tpp

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
  if (state.at(State::off) || state.at(State::on)) return; // nothing to update!
  if (periods == 0) {
    onPeriodsFinished();
    return;
  }
  if (state.at(State::fadingHigh) || state.at(State::fadingLow)) {
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
  if (state.at(State::fadingLow)) {
    if (periods > 0) --periods;
    if (periods == 0) off();
    else led.fade(highBrightness, highInterval);
  } else { // state.at(State::fadingHigh)
    led.fade(lowBrightness, lowInterval);
  }
}

void LED::updateBlinking() {
  if (state.at(State::blinkingHigh) && state.currentDuration() < highInterval) return;
  if (state.at(State::blinkingLow) && state.currentDuration() < lowInterval) return;
  if (state.at(State::blinkingLow)) {
    if (periods > 0) --periods;
    if (periods == 0) {
      off();
      return;
    }
    led.set_value(highBrightness);
    state.update(State::blinkingHigh);
  } else { // state.at(State::blinkingHigh)
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
  if (state.at(State::off) || state.at(State::on)) return; // nothing to update!
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
  if (state.at(State::blinkingHigh) && state.currentDuration() < highInterval) return;
  if (state.at(State::blinkingLow) && state.currentDuration() < lowInterval) return;
  if (state.at(State::blinkingLow)) {
    if (periods > 0) --periods;
    if (periods == 0) {
      off();
      return;
    }
    digitalWrite(ledPin, HIGH);
    state.update(State::blinkingHigh);
  } else { // state.at(State::blinkingHigh)
    digitalWrite(ledPin, LOW);
    state.update(State::blinkingLow);
  }
}

} }

#endif

