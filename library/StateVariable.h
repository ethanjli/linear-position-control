#ifndef StateVariable_h
#define StateVariable_h

#include <elapsedMillis.h>

namespace LinearPositionControl {

template <class State>
class StateVariable {
  public:
    void setup(State initialState);
    void update(State nextState);

    State current() const;
    State previous() const;
    State previousDistinct() const;

    unsigned long currentDuration() const;
    unsigned long previousDistinctDuration() const;

    bool justEntered(State state) const;
    bool justChanged() const;

  private:
    bool setupCompleted = false;

    State currentState;
    State previousState;
    State previousDistinctState;

    elapsedMillis currentTimer;
    unsigned long previousDistinctTimer;
};

}

#include "StateVariable.tpp"

#endif

