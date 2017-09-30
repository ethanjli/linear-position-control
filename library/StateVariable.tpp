#ifndef StateVariable_tpp
#define StateVariable_tpp

namespace LinearPositionControl {

// StateVariable

template <class State>
void StateVariable<State>::setup(State initialState) {
  if (setupCompleted) return;

  currentState = initialState;
  previousState = initialState;
  previousDistinctState = initialState;

  setupCompleted = true;
}

template <class State>
void StateVariable<State>::update(State nextState) {
  previousState = currentState;
  if (currentState != nextState) {
    previousDistinctState = currentState;
    previousDistinctTimer = currentTimer;
    currentTimer = 0;
  }
  currentState = nextState;
}

template <class State>
State StateVariable<State>::current() const {
  return currentState;
}

template <class State>
State StateVariable<State>::previous() const {
  return previousState;
}

template <class State>
State StateVariable<State>::previousDistinct() const {
  return previousDistinctState;
}

template <class State>
unsigned long StateVariable<State>::currentDuration() const {
  return currentTimer;
}

template <class State>
unsigned long StateVariable<State>::previousDistinctDuration() const {
  return previousDistinctTimer;
}

}

#endif

