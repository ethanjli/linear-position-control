#ifndef LPC_Control_CumulativeLinearPosition_h
#define LPC_Control_CumulativeLinearPosition_h

#include "Components/Motors.h"
#include "Components/AngleSensor.h"
#include "Signal/Smoothing.h"
#include "LinearPosition.h"

namespace LinearPositionControl { namespace Control {

using CumulativeLinearActuator = LinearActuator<Components::AngleSensor>;

namespace Calibration { namespace States {
  enum class Position : uint8_t {
    calibrated,
    uncalibrated,
    initializing,
    calibrating
  };
}}

// Note: this implementation is not robust at all, and should only be used if
// memory limitations prohibit the use of smoothing!
class CumulativePositionCalibrator {
  public:
    CumulativePositionCalibrator(
        CumulativeLinearActuator &actuator,
        Components::MotorSpeed calibrationSpeed = 255
    );

    using State = Calibration::States::Position;

    void setup();
    void update();

    StateVariable<State> state;
    const Components::MotorSpeed calibrationSpeed;

    bool calibrated() const;

  private:
    bool setupCompleted = false;
    CumulativeLinearActuator &actuator;
    StateVariable<int> discretePosition;

    unsigned int limitTimeout = 100;

    void updateUncalibrated();
    void updateInitializing();
    void updateCalibrating();
    void onPositionCalibrated();
};

class SmoothedCumulativePositionCalibrator {
  public:
    SmoothedCumulativePositionCalibrator(
        CumulativeLinearActuator &actuator,
        Signal::ContinuousSmoother &smoother,
        Components::MotorSpeed calibrationSpeed = 255
    );

    using State = Calibration::States::Position;

    void setup();
    void update();

    StateVariable<State> state;
    const Components::MotorSpeed calibrationSpeed;

    bool calibrated() const;

  private:
    bool setupCompleted = false;
    CumulativeLinearActuator &actuator;
    Signal::ContinuousSmoother &smoother;

    unsigned int limitTimeout = 100;

    void updateUncalibrated();
    void updateInitializing();
    void updateCalibrating();
    void onPositionCalibrated();
};

} }

#include "CumulativeLinearPosition.tpp"

#endif

