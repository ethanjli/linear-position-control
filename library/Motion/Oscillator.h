#ifndef Motion_Oscillator_h
#define Motion_Oscillator_h

#include "Motors.h"
#include "Limits.h"

namespace LinearPositionControl { namespace Motion {

namespace States {
  enum class Oscillator : uint8_t {
    initializing,
    operating
  };
}

template <class Limits>
class Oscillator {
  public:
    Oscillator(Components::Motor &motor, Limits &limits);

    using States = States::Oscillator;

    void setup();
    void update();

  private:
    bool setupCompleted = false;

    States state;

    Components::Motor &motor;
    Limits &limits;

    void updateInitializing();
    void updateOperating();
};

} }

#include "Oscillator.tpp"

#endif

