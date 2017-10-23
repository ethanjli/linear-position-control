#ifndef Motion_Hysteresis_h
#define Motion_Hysteresis_h

#include "StateVariable.h"
#include "Motors.h"

namespace LinearPositionControl { namespace Motion {

namespace States {
  enum class Hysteresis : uint8_t {
    targeting,
    adjusting,
    maintaining
  };
}

template <class PositionTracker>
class Hysteresis {
  public:
    Hysteresis(Components::Motor &motor, PositionTracker &positionTracker);

    using State = States::Hysteresis;
    StateVariable<State> state;

    StateVariable<int> targetPosition;
    Components::MotorSpeed adjustmentSpeed = 255;

    void setup();
    void update();

  private:
    bool setupCompleted = false;

    Components::Motor &motor;
    PositionTracker &positionTracker;

    void updateTargeting();
    void updateAdjusting();
    void updateMaintaining();
};

} }

#include "Hysteresis.tpp"

#endif

