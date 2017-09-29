#ifndef Oscillator_h
#define Oscillator_h

#include "Motors.h"
#include "Limits.h"

template <bool debug_serial>
class Oscillator {
  public:
    Oscillator(Motor &motor, AbsoluteLimits &limits); // FIXME: we shouldn't assume AbsoluteLimits

    void setup();
    void update();

  private:
    bool setupCompleted = false;

    Motor &motor;
    AbsoluteLimits &limits;
};

#include "Oscillator.tpp"

#endif

