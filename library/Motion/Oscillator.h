#ifndef Oscillator_h
#define Oscillator_h

#include <elapsedMillis.h>

#include "Motors.h"
#include "Limits.h"

template <bool debug_serial>
class Oscillator {
  public:
    Oscillator(Motor &motor, AbsoluteLimits &limits);

    void setup();
    void update();

  private:
    bool setupCompleted = false;

    Motor &motor;
    AbsoluteLimits &limits;

    elapsedMillis motorStallTimer;
    unsigned int motorStallTimeout = 250;
};

#include "Oscillator.tpp"

#endif

