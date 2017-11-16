#ifndef Tracking_Position_Discrete_h
#define Tracking_Position_Discrete_h

#include "StateVariable.h"
#include "Motors.h"
#include "Limits.h"
#include "Tracking/Limits.h"
#include "OpticalSensor.h"
#include "Position.h"

namespace LinearPositionControl { namespace Tracking { namespace Position {

template<class Limits, class EdgeCounter>
class Discrete {
  public:
    Discrete(
        Components::Motor &motor,
        Tracking::AbsoluteLimits<Limits> &limitsTracker,
        EdgeCounter &edgeCounter
    );

    using State = States::Position;
    using LimitsTracker = Tracking::AbsoluteLimits<Limits>;

    void setup();
    void update();

    StateVariable<State> state;
    StateVariable<int> position;
    Components::MotorSpeed relocalizationSpeed = 255;
    int forwardsEdgesSinceLastLimit = 0;
    int backwardsEdgesSinceLastLimit = 0;

    void updateNumTotalEdges(int numEdges);
    int getNumTotalEdges() const;
    bool atLeftLimit() const;
    bool atRightLimit() const;
    int mapPositionTo(int position, int toMin, int toMax) const;
    int mapPositionFrom(int position, int fromMin, int fromMax) const;

  private:
    bool setupCompleted = false;

  protected:
    Components::Motor &motor;
    LimitsTracker &limitsTracker;
    EdgeCounter &edgeCounter;

    unsigned int limitSwitchTimeout = 250;

    int numTotalEdges = 0;

    void updateUnlocalized();
    void updateLocalizing();
    void updateTracking();
    void updateMotorPosition(bool setup = false);
    int inferMotorPosition(bool &error);

    void onLimitPressed(Components::States::Limits state);
};

} } }

#include "Discrete.tpp"

#endif

