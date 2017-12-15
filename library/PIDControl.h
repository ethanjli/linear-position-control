#ifndef PID_h
#define PID_h

#include <PID_v1.h>

#include "StateVariable.h"

namespace LinearPositionControl {

namespace PIDModes {
  const int inputProportionality = P_ON_M;
  const int errorProportionality = P_ON_E;
  const int directOutput = DIRECT;
  const int reverseOutput = REVERSE;
  const int controlling = AUTOMATIC;
  const int notControlling = MANUAL;
}

template <class InputType, class OutputType>
class PIDController {
  public:
    PIDController(
        StateVariable<InputType> &inputStateVariable, double kp, double kd, double ki,
        OutputType minOutput = 0, OutputType maxOutput = 255,
        int sampleTime = 100, int outputMode = PIDModes::directOutput,
        int proportionalityMode = PIDModes::errorProportionality
    );

    void setup();
    void update();

    StateVariable<OutputType> output;

    void setKp(double newKp);
    void setKd(double newKd);
    void setKi(double newKi);
    void setCoefficients(double newKp, double newKd, double newKi);
    void setMinOutput(OutputType minLimit);
    void setMaxOutput(OutputType maxLimit);
    void setSampleTime(int time);

    void enable();
    void disable();

    void setSetpoint(InputType newSetpoint);
    InputType getInput() const;

  private:
    bool setupCompleted = false;

    double kp;
    double kd;
    double ki;
    double minOutput;
    double maxOutput;
    int sampleTime;

    double pidInput = 0;
    double pidOutput = 0;
    double pidSetpoint = 0;

    StateVariable<InputType> &input;
    PID pid;
    InputType rawInput;
};

using DiscretePID = PIDController<int, int>;

}

#include "PIDControl.tpp"

#endif

