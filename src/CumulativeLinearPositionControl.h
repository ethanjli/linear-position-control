#ifndef CumulativeLinearPositionControl_h
#define CumulativeLinearPositionControl_h

#include "Motors.h"
#include "AngleSensor.h"
#include "PIDControl.h"
#include "Smoothing.h"

namespace LinearPositionControl {

class CumulativeLinearActuator {
  public:
    CumulativeLinearActuator(
        Components::Motors &motors, MotorPort motorPort,
        uint8_t angleSensorPort, int minPosition, int maxPosition,
        double pidKp, double pidKd, double pidKi, int pidSampleTime,
        bool swapMotorPolarity, int feedforward,
        int brakeLowerThreshold, int brakeUpperThreshold,
        int minDuty = -255, int maxDuty = 255
    );

    using Position = float;

    Components::Motors &motors;
    Components::Motor motor;
    Components::AngleSensor angleSensor;
    SimpleStateVariable<Position> &position;
    PIDController<Position, int> pid;
    Components::MotorSpeedAdjuster speedAdjuster;
    const bool swapMotorPolarity;

    void setup();
    void update();

    void freeze(bool brake = true);
    void unfreeze();

  private:
    bool frozen;
};

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
        ContinuousSmoother &smoother,
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
    ContinuousSmoother &smoother;

    unsigned int limitTimeout = 100;

    void updateUncalibrated();
    void updateInitializing();
    void updateCalibrating();
    void onPositionCalibrated();
};

}

#endif

