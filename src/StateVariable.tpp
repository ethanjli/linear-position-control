#ifndef StateVariable_tpp
#define StateVariable_tpp

namespace LinearPositionControl {

// StateVariable

template <class State>
void StateVariable<State>::setup(State initialState, bool force) {
  if (!force && setupCompleted) return;

  currentState = initialState;
  previousState = initialState;
  previousDistinctState = initialState;

  setupCompleted = true;
}

template <class State>
void StateVariable<State>::update(State nextState, bool force) {
  previousState = currentState;
  if (force || currentState != nextState) {
    previousDistinctState = currentState;
    previousDistinctTimer = currentTimer;
    previousDistinctTimerMicros = currentTimerMicros;
    currentTimer = 0;
    currentTimerMicros = 0;
  }
  currentState = nextState;
}

template <class State>
inline State StateVariable<State>::current() const {
  return currentState;
}

template <class State>
inline State StateVariable<State>::previous() const {
  return previousState;
}

template <class State>
inline State StateVariable<State>::previousDistinct() const {
  return previousDistinctState;
}

template <class State>
inline unsigned long StateVariable<State>::currentDuration() const {
  return currentTimer;
}

template <class State>
inline unsigned long StateVariable<State>::previousDistinctDuration() const {
  return previousDistinctTimer;
}

template <class State>
inline unsigned long StateVariable<State>::currentDurationMicros() const {
  return currentTimerMicros;
}

template <class State>
inline unsigned long StateVariable<State>::previousDistinctDurationMicros() const {
  return previousDistinctTimerMicros;
}

template <class State>
inline bool StateVariable<State>::justEntered(State state) const {
  return currentState == state && previousState != currentState;
}

template <class State>
inline bool StateVariable<State>::justChanged() const {
  return currentState != previousState;
}

template <class State>
inline bool StateVariable<State>::settled(unsigned long settleThreshold) const {
  return currentTimer >= settleThreshold;
}

template <class State>
inline bool StateVariable<State>::settledAt(State state, unsigned long settleThreshold) const {
  return currentState == state && currentTimer >= settleThreshold;
}

}

#endif

