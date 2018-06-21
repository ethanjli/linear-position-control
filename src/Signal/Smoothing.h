#ifndef LPC_Signal_Smoothing_h
#define LPC_Signal_Smoothing_h

#include <ResponsiveAnalogRead.h>

#include "StateVariable.h"

namespace LinearPositionControl { namespace Signal {

template <class InputType, class OutputType>
class Smoother {
  public:
    Smoother(
        SimpleStateVariable<InputType> &inputStateVariable,
        float snapMultiplier = 0.01, int analogResolution = 1023,
        bool enableSleep = true, float activityThreshold = 4.0
    );

    void setup();
    void update();

    StateVariable<OutputType> output;

    void enableSleep();
    void disableSleep();

    InputType getInput() const;
    InputType getMaxInput() const;

  private:
    bool setupCompleted = false;

    int analogResolution;
    float activityThreshold;

    SimpleStateVariable<InputType> &input;
    ResponsiveAnalogRead smoother;

    InputType rawInput = 0;
    InputType minInput;
    InputType maxInput;
};

using DiscreteSmoother = Smoother<int, int>;
using ContinuousSmoother = Smoother<float, int>;

} }

#include "Smoothing.tpp"

#endif

