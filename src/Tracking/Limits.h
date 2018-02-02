#ifndef Tracking_Limits_h
#define Tracking_Limits_h

#include "StateVariable.h"
#include "Motors.h"
#include "Limits.h"

namespace LinearPositionControl { namespace Tracking {

template<class Limits>
class AbsoluteLimits {
  public:
    AbsoluteLimits(Components::Motor &motor, Limits &limits);

    using State = Components::States::Limits;

    void setup();
    void update();

    StateVariable<State> state;
    StateVariable<State> lastLimit;

  private:
    bool setupCompleted = false;

  protected:
    Components::Motor &motor;
    Limits &limits;

    Components::MotorDirection limitSwitchPressDirection;
};

} }

#include "Limits.tpp"

#endif

