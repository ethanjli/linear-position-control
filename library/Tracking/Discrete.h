#ifndef Tracking_Discrete_h
#define Tracking_Discrete_h

#include <elapsedMillis.h>

#include "StateVariable.h"
#include "Motors.h"
#include "Limits.h"
#include "OpticalSensor.h"

namespace LinearPositionControl { namespace Tracking {

namespace States {
  enum class Discrete : uint8_t {
    uncalibrated,
    unlocalized,
    localizing,
    tracking
  };
}

template<class Limits, class EdgeCounter>
class Discrete {
  public:
    Discrete(Components::Motor &motor, Limits &limits, EdgeCounter &edgeCounter);

    using State = States::Discrete;

    void setup();
    void update();

    StateVariable<State> state;
    StateVariable<int> position;

    void updateNumTotalEdges(int numEdges);
    bool atLeftLimit() const;
    bool atRightLimit() const;

  private:
    bool setupCompleted = false;

  protected:
    Components::Motor &motor;
    Limits &limits;
    EdgeCounter &edgeCounter;

    elapsedMillis limitSwitchTimer;
    unsigned int limitSwitchTimeout = 250;

    int numTotalEdges = 0;

    void updateUnlocalized();
    void updateLocalizing();
    void updateTracking();
    void onPositionLocalized();
    void updateMotorPosition(bool setup = false);
    int inferMotorPosition();
};

} }

#include "Discrete.tpp"

#endif

