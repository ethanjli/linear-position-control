#ifndef LPC_Signal_Smoothing_tpp
#define LPC_Signal_Smoothing_tpp

namespace LinearPositionControl { namespace Signal {

// Smoother

template <class InputType, class OutputType>
Smoother<InputType, OutputType>::Smoother(
    SimpleStateVariable<InputType> &inputStateVariable,
    float snapMultiplier, int analogResolution,
    bool enableSleep, float activityThreshold
) :
  analogResolution(analogResolution), activityThreshold(activityThreshold),
  input(inputStateVariable), smoother(0, enableSleep, snapMultiplier),
  maxInput(analogResolution)
{}

template <class InputType, class OutputType>
void Smoother<InputType, OutputType>::setup() {
  if (setupCompleted) return;

  smoother.setActivityThreshold(activityThreshold);
  smoother.update(input.current);
  output.setup(smoother.getValue());

  setupCompleted = true;
}

template <class InputType, class OutputType>
void Smoother<InputType, OutputType>::update() {
  rawInput = input.current;
  smoother.update(rawInput);
  output.update(smoother.getValue());
}

template <class InputType, class OutputType>
void Smoother<InputType, OutputType>::enableSleep() {
  smoother.enableSleep();
}

template <class InputType, class OutputType>
void Smoother<InputType, OutputType>::disableSleep() {
  smoother.disableSleep();
}

template <class InputType, class OutputType>
InputType Smoother<InputType, OutputType>::getInput() const {
  return rawInput;
}

template <class InputType, class OutputType>
InputType Smoother<InputType, OutputType>::getMaxInput() const {
  return maxInput;
}

} }

#endif

