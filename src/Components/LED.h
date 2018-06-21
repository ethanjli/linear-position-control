#ifndef LPC_Components_LED_h
#define LPC_Components_LED_h

#include <LEDFader.h>

#include "StateVariable.h"

namespace LinearPositionControl { namespace Components {

namespace States {
  enum class LED : uint8_t {
    off,
    on,
    blinkingHigh,
    blinkingLow,
    fadingHigh,
    fadingLow
  };
}

class LED {
  public:
    LED(uint8_t ledPin);

    using State = States::LED;

    void setup();
    void update();

    StateVariable<State> state;

    uint8_t lowBrightness = 0;
    uint8_t highBrightness = 255;
    unsigned int highInterval = 1000;
    unsigned int lowInterval = 1000;
    int periods = -1;

    void off();
    void on(uint8_t Brightness);
    void on();
    void blink();
    void fade();

  private:
    bool setupCompleted = false;

    LEDFader led;
    const uint8_t ledPin;

    void updateFading();
    void updateBlinking();
    void onPeriodsFinished();
};

class SimpleLED {
  public:
    SimpleLED(uint8_t ledPin);

    using State = States::LED;

    void setup();
    void update();

    StateVariable<State> state;

    unsigned int highInterval = 1000;
    unsigned int lowInterval = 1000;
    int periods = -1;

    void off();
    void on();
    void blink();

  private:
    bool setupCompleted = false;

    const uint8_t ledPin;

    void updateBlinking();
    void onPeriodsFinished();
};

} }

#include "LED.tpp"

#endif

