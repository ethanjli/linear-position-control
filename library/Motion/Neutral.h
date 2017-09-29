#ifndef Neutral_h
#define Neutral_h

#include "Motors.h"

template <class Limits>
class Neutral {
  public:
    Neutral(Motor &motor);

    void setup();
    void update();

  private:
    bool setupCompleted = false;

    Motor &motor;
};

#include "Neutral.tpp"

#endif

