#ifndef Smoothing_h
#define Smoothing_h

#include <ResponsiveAnalogRead.h>

#include "StateVariable.h"

namespace LinearPositionControl {

template <class InputType, class OutputType>
class Smoother {
  public:
    Smoother(
        StateVariable<InputType> &inputStateVariable,
        float snapMultiplier = 0.01, int analogResolution = 1023,
        bool enableSleep = true, float activityThreshold = 4.0
    );

    void setup();
    void update();

    StateVariable<OutputType> output;

    void enableSleep();
    void disableSleep();

    InputType getInput() const;

  private:
    bool setupCompleted = false;

    int analogResolution;
    float activityThreshold;

    StateVariable<InputType> &input;
    ResponsiveAnalogRead smoother;

    InputType rawInput = 0;
    InputType minInput;
    InputType maxInput;
};

using DiscreteSmoother = Smoother<int, int>;
using ContinuousSmoother = Smoother<float, int>;

}

#include "Smoothing.tpp"

#endif

