#ifndef PIDControl_tpp
#define PIDControl_tpp

namespace LinearPositionControl {

// PID

template <class InputType, class OutputType>
PIDController<InputType, OutputType>::PIDController(
    StateVariable<InputType> &inputStateVariable, double kp, double kd, double ki,
    OutputType minOutput, OutputType maxOutput, int sampleTime,
    InputType minInput, InputType maxInput, int outputMode, int proportionalityMode
) :
  minOutput(minOutput), maxOutput(maxOutput),
  input(inputStateVariable),
  pid(&pidInput, &pidOutput, &pidSetpoint, kp, kd, ki, proportionalityMode, outputMode),
  minInput(minInput), maxInput(maxInput) {
  pid.SetSampleTime(sampleTime);
}

template <class InputType, class OutputType>
void PIDController<InputType, OutputType>::setup() {
  if (setupCompleted) return;

  pid.SetOutputLimits(minOutput, maxOutput);
  rawInput = input.current();
  pidInput = rawInput;
  output.setup(0);

  setupCompleted = true;
}

template <class InputType, class OutputType>
void PIDController<InputType, OutputType>::update() {
  rawInput = input.current();
  pidInput = rawInput;
  pid.Compute();
  output.update((OutputType) pidOutput);
}

template <class InputType, class OutputType>
void PIDController<InputType, OutputType>::setKp(double newKp) {
  pid.SetTunings(newKp, pid.GetKi(), pid.GetKd());
}

template <class InputType, class OutputType>
void PIDController<InputType, OutputType>::setKd(double newKd) {
  pid.SetTunings(pid.GetKp(), pid.GetKi(), newKd);
}

template <class InputType, class OutputType>
void PIDController<InputType, OutputType>::setKi(double newKi) {
  pid.SetTunings(pid.GetKp(), newKi, pid.GetKd());
}

template <class InputType, class OutputType>
void PIDController<InputType, OutputType>::setMinInput(InputType minLimit) {
  minInput = minLimit;
}

template <class InputType, class OutputType>
void PIDController<InputType, OutputType>::setMaxInput(InputType maxLimit) {
  maxInput = maxLimit;
}

template <class InputType, class OutputType>
void PIDController<InputType, OutputType>::setMinOutput(OutputType minLimit) {
  minOutput = minLimit;
  pid.SetOutputLimits(minOutput, maxOutput);
}

template <class InputType, class OutputType>
void PIDController<InputType, OutputType>::setMaxOutput(OutputType maxLimit) {
  maxOutput = maxLimit;
  pid.SetOutputLimits(minOutput, maxOutput);
}

template <class InputType, class OutputType>
void PIDController<InputType, OutputType>::setSampleTime(int time) {
  pid.SetSampleTime(time);
}

template <class InputType, class OutputType>
void PIDController<InputType, OutputType>::enable() {
  pid.SetMode(PIDModes::controlling);
}

template <class InputType, class OutputType>
void PIDController<InputType, OutputType>::disable() {
  pid.SetMode(PIDModes::notControlling);
}

template <class InputType, class OutputType>
void PIDController<InputType, OutputType>::setSetpoint(InputType newSetpoint) {
  if (minInput < maxInput) newSetpoint = constrain(newSetpoint, minInput, maxInput);
  setpoint.update(newSetpoint);
  pidSetpoint = newSetpoint;
  enable();
}

template <class InputType, class OutputType>
InputType PIDController<InputType, OutputType>::getInput() const {
  return rawInput;
}

template <class InputType, class OutputType>
InputType PIDController<InputType, OutputType>::getMinInput() const {
  return minInput;
}

template <class InputType, class OutputType>
InputType PIDController<InputType, OutputType>::getMaxInput() const {
  return maxInput;
}

template <class InputType, class OutputType>
double PIDController<InputType, OutputType>::getMinOutput() const {
  return minOutput;
}

template <class InputType, class OutputType>
double PIDController<InputType, OutputType>::getMaxOutput() const {
  return maxOutput;
}

template <class InputType, class OutputType>
double PIDController<InputType, OutputType>::getKp() {
  return pid.GetKp();
}

template <class InputType, class OutputType>
double PIDController<InputType, OutputType>::getKd() {
  return pid.GetKd();
}

template <class InputType, class OutputType>
double PIDController<InputType, OutputType>::getKi() {
  return pid.GetKi();
}

}

#endif

