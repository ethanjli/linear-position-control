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
  kp(kp), kd(kd), ki(ki), minOutput(minOutput), maxOutput(maxOutput),
  sampleTime(sampleTime), input(inputStateVariable),
  pid(&pidInput, &pidOutput, &pidSetpoint, kp, kd, ki, proportionalityMode, outputMode),
  minInput(minInput), maxInput(maxInput) {}

template <class InputType, class OutputType>
void PIDController<InputType, OutputType>::setup() {
  if (setupCompleted) return;

  pid.SetOutputLimits(minOutput, maxOutput);
  pid.SetSampleTime(sampleTime);
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
  kp = newKp;
  pid.SetTunings(kp, kd, ki);
}

template <class InputType, class OutputType>
void PIDController<InputType, OutputType>::setKd(double newKd) {
  kd = newKd;
  pid.SetTunings(kp, kd, ki);
}

template <class InputType, class OutputType>
void PIDController<InputType, OutputType>::setKi(double newKi) {
  ki = newKi;
  pid.SetTunings(kp, kd, ki);
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
  sampleTime = time;
  pid.SetSampleTime(sampleTime);
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

}

#endif

