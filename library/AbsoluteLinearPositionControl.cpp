#include "AbsoluteLinearPositionControl.h"

namespace LinearPositionControl {

// AbsoluteLinearActuator

AbsoluteLinearActuator::AbsoluteLinearActuator(
    Components::Motors &motors, MotorPort motorPort,
    uint8_t potentiometerPin, int minPosition, int maxPosition,
    double pidKp, double pidKd, double pidKi, int pidSampleTime,
    bool swapMotorPolarity, int feedforward, int brakeThreshold
) :
  motors(motors),
  motor(motors, motorPort),
  potentiometer(potentiometerPin),
  pid(
    potentiometer.state, pidKp, pidKd, pidKi,
    -255 - feedforward, 255 - feedforward, pidSampleTime,
    minPosition, maxPosition
  ),
  speedAdjuster(pid.output, feedforward, brakeThreshold),
  swapMotorPolarity(swapMotorPolarity)
{
}

void AbsoluteLinearActuator::setup() {
  motors.setup();
  if (swapMotorPolarity) motor.swapDirections();
  potentiometer.setup();
  pid.setup();
}

void AbsoluteLinearActuator::update() {
  potentiometer.update();
  pid.update();
  speedAdjuster.update();
  motor.run(speedAdjuster.output.current());
}

}

