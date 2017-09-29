#ifndef Oscillator_h
#define Oscillator_h

#include "Motors.h"
#include "Limits.h"

namespace States {
  namespace Motion {
    enum class Oscillator : uint8_t {
      initializing,
      operating
    };
  }
}

template <class Limits, bool debug_serial>
class Oscillator {
  public:
    Oscillator(Motor &motor, Limits &limits);

    void setup();
    void update();

  private:
    bool setupCompleted = false;

    States::Motion::Oscillator state;

    Motor &motor;
    Limits &limits;

    void updateInitializing();
    void updateOperating();
};

#include "Oscillator.tpp"

#endif

