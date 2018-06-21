#ifndef LPC_Control_PID_h
#define LPC_Control_PID_h

#include <PID_v1.h>

#include "StateVariable.h"

namespace LinearPositionControl { namespace Control {

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
        const InputType &inputValue, double kp, double kd, double ki,
        OutputType minOutput = 0, OutputType maxOutput = 255, int sampleTime = 100,
        InputType minInput = 0, InputType maxInput = 0,
        int outputMode = PIDModes::directOutput,
        int proportionalityMode = PIDModes::errorProportionality
    );

    void setup();
    void update();

    OutputType output = 0;
    StateVariable<InputType> setpoint;

    void setKp(double newKp);
    void setKd(double newKd);
    void setKi(double newKi);
    void setMinInput(InputType minLimit);
    void setMaxInput(InputType maxLimit);
    void setMinOutput(OutputType minLimit);
    void setMaxOutput(OutputType maxLimit);
    void setSampleTime(int time); // this is int because the Arduino PID library uses int

    void enable();
    void disable();

    void setSetpoint(InputType newSetpoint);
    InputType getSetpoint() const;
    InputType getMinInput() const;
    InputType getMaxInput() const;
    double getMinOutput() const;
    double getMaxOutput() const;
    int getSampleTime() const;
    // These methods aren't const because the Arduino PID library isn't const-correct either :(
    double getKp();
    double getKd();
    double getKi();

  private:
    bool setupCompleted = false;

    double minOutput;
    double maxOutput;
    int sampleTime;

    double pidInput = 0;
    double pidOutput = 0;
    double pidSetpoint = 0;

    const InputType &input;
    PID pid;

    InputType minInput;
    InputType maxInput;
};

using DiscretePID = PIDController<int, int>;
using ContinuousPID = PIDController<float, int>;

} }

#include "PID.tpp"

#endif

