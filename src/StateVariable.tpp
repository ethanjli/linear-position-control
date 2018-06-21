#ifndef StateVariable_tpp
#define StateVariable_tpp

namespace LinearPositionControl {

// StateVariable

template <class State>
void StateVariable<State>::setup(State initialState, bool force) {
  if (!force && setupCompleted) return;

  currentState = initialState;
  previousState = initialState;

  setupCompleted = true;
}

template <class State>
void StateVariable<State>::update(State nextState, bool force) {
  previousState = currentState;
  if (force || currentState != nextState) {
    currentTimer = 0;
  }
  currentState = nextState;
}

template <class State>
inline unsigned long StateVariable<State>::currentDuration() const {
  return currentTimer;
}

template <class State>
inline bool StateVariable<State>::at(State state) const {
  return currentState == state;
}

template <class State>
inline bool StateVariable<State>::previouslyAt(State state) const {
  return previousState == state;
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

// SimpleStateVariable

template <class State>
void SimpleStateVariable<State>::setup(State initialState, bool force) {
  if (!force && setupCompleted) return;

  currentState = initialState;
  previousState = initialState;

  setupCompleted = true;
}

template <class State>
void SimpleStateVariable<State>::update(State nextState, bool force) {
  previousState = currentState;
  currentState = nextState;
}

template <class State>
inline bool SimpleStateVariable<State>::at(State state) const {
  return currentState == state;
}

template <class State>
inline bool SimpleStateVariable<State>::previouslyAt(State state) const {
  return previousState == state;
}

template <class State>
inline bool SimpleStateVariable<State>::justEntered(State state) const {
  return currentState == state && previousState != currentState;
}

template <class State>
inline bool SimpleStateVariable<State>::justChanged() const {
  return currentState != previousState;
}

}

#endif

