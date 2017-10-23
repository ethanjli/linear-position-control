#ifndef Motion_Oscillator_h
#define Motion_Oscillator_h

#include "StateVariable.h"
#include "Motors.h"

namespace LinearPositionControl { namespace Motion {

namespace States {
  enum class Oscillator : uint8_t {
    initializing,
    operating
  };
}

template <class PositionTracker>
class Oscillator {
  public:
    Oscillator(Components::Motor &motor, PositionTracker &positionTracker);

    using State = States::Oscillator;

    void setup();
    void update();

  private:
    bool setupCompleted = false;

    StateVariable<State> state;

    Components::Motor &motor;
    PositionTracker &positionTracker;

    void updateInitializing();
    void updateOperating();
};

} }

#include "Oscillator.tpp"

#endif

