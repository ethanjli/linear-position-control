#ifndef AbsoluteLinearPositionControl_h
#define AbsoluteLinearPositionControl_h

#include "Motors.h"
#include "AnalogSensor.h"
#include "PIDControl.h"

namespace LinearPositionControl {

class AbsoluteLinearActuator {
  public:
    AbsoluteLinearActuator(
        Components::Motors &motors, MotorPort motorPort,
        uint8_t potentiometerPin, int minPosition, int maxPosition,
        double pidKp, double pidKd, double pidKi, int pidSampleTime,
        bool swapMotorPolarity, int feedforward,
        int brakeLowerThreshold, int brakeUpperThreshold,
        int minDuty = -255, int maxDuty = 255
    );

    using Position = int;

    Components::Motors &motors;
    Components::Motor motor;
    Components::AnalogSensor potentiometer;
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

}

#include "AbsoluteLinearPositionControl.tpp"

#endif

