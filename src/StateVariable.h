#ifndef StateVariable_h
#define StateVariable_h

#include <elapsedMillis.h>

namespace LinearPositionControl {

template <class State>
class StateVariable {
  public:
    void setup(State initialState, bool force = false);
    void update(State nextState, bool force = false);

    const State &current = currentState;
    const State &previous = previousState;

    unsigned long currentDuration() const;

    bool at(State state) const;
    bool previouslyAt(State state) const;
    bool justEntered(State state) const;
    bool justChanged() const;
    bool settled(unsigned long settleThreshold) const;
    bool settledAt(State state, unsigned long settleThreshold) const;

  private:
    bool setupCompleted = false;

    State currentState;
    State previousState;

    elapsedMillis currentTimer;
};

template <class State>
class SimpleStateVariable {
  public:
    void setup(State initialState, bool force = false);
    void update(State nextState, bool force = false);

    const State &current = currentState;
    const State &previous = previousState;

    bool at(State state) const;
    bool previouslyAt(State state) const;
    bool justEntered(State state) const;
    bool justChanged() const;

  private:
    bool setupCompleted = false;

    State currentState;
    State previousState;
};

}

#include "StateVariable.tpp"

#endif

