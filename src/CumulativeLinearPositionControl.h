#ifndef CumulativeLinearPositionControl_h
#define CumulativeLinearPositionControl_h

#include "Motors.h"
#include "AngleSensor.h"
#include "PIDControl.h"

namespace LinearPositionControl {

class CumulativeLinearActuator {
  public:
    CumulativeLinearActuator(
        Components::Motors &motors, MotorPort motorPort,
        int minPosition, int maxPosition,
        double pidKp, double pidKd, double pidKi, int pidSampleTime,
        bool swapMotorPolarity, int feedforward,
        int brakeLowerThreshold, int brakeUpperThreshold,
        int minDuty = -255, int maxDuty = 255
    );

    Components::Motors &motors;
    Components::Motor motor;
    Components::AngleSensor angleSensor;
    ContinuousPID pid;
    Components::MotorSpeedAdjuster speedAdjuster;
    const bool swapMotorPolarity;

    void setup();
    void update();

    void freeze();
    void unfreeze();
};

namespace Calibration { namespace States {
  enum class Position : uint8_t {
    calibrated,
    uncalibrated,
    initializing,
    calibrating
  };
}}

class CumulativePositionCalibrator {
  public:
    CumulativePositionCalibrator(CumulativeLinearActuator &actuator);

    using State = Calibration::States::Position;

    void setup();
    void update();

    StateVariable<State> state;
    const Components::MotorSpeed calibrationSpeed = 255;

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

}

#endif

