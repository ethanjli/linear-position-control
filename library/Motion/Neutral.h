#ifndef Motion_Neutral_h
#define Motion_Neutral_h

#include "Motors.h"

namespace LinearPositionControl { namespace Motion {

template <class Limits>
class Neutral {
  public:
    Neutral(Components::Motor &motor);

    void setup();
    void update();

  private:
    bool setupCompleted = false;

    Components::Motor &motor;
};

} }

#include "Neutral.tpp"

#endif

