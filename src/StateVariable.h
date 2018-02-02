#ifndef StateVariable_h
#define StateVariable_h

#include <elapsedMillis.h>

namespace LinearPositionControl {

template <class State>
class StateVariable {
  public:
    void setup(State initialState, bool force = false);
    void update(State nextState, bool force = false);

    State current() const;
    State previous() const;
    State previousDistinct() const;

    unsigned long currentDuration() const;
    unsigned long previousDistinctDuration() const;
    unsigned long currentDurationMicros() const;
    unsigned long previousDistinctDurationMicros() const;

    bool justEntered(State state) const;
    bool justChanged() const;
    bool settled(unsigned long settleThreshold) const;
    bool settledAt(State state, unsigned long settleThreshold) const;

  private:
    bool setupCompleted = false;

    State currentState;
    State previousState;
    State previousDistinctState;

    elapsedMillis currentTimer;
    elapsedMicros currentTimerMicros;
    unsigned long previousDistinctTimer;
    unsigned long previousDistinctTimerMicros;
};

}

#include "StateVariable.tpp"

#endif

