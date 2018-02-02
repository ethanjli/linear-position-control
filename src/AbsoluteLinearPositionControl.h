#ifndef AbsoluteLinearPositionControl_h
#define AbsoluteLinearPositionControl_h

#include "Motors.h"
#include "DebouncedButton.h"
#include "AnalogSensor.h"
#include "PIDControl.h"

namespace LinearPositionControl {

class AbsoluteLinearActuator {
  public:
    AbsoluteLinearActuator(
        Components::Motors &motors, MotorPort motorPort,
        uint8_t potentiometerPin, int minPosition, int maxPosition,
        double pidKp, double pidKd, double pidKi, int pidSampleTime,
        bool swapMotorPolarity, int feedforward, int brakeThreshold
    );

    Components::Motors &motors;
    Components::Motor motor;
    Components::AnalogSensor potentiometer;
    DiscretePID pid;
    Components::MotorSpeedAdjuster speedAdjuster;
    const bool swapMotorPolarity;

    void setup();
    void update();
};

}

#endif

