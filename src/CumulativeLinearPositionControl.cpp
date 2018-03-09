#include "CumulativeLinearPositionControl.h"

namespace LinearPositionControl {

// CumulativeLinearActuator

CumulativeLinearActuator::CumulativeLinearActuator(
    Components::Motors &motors, MotorPort motorPort,
    int minPosition, int maxPosition,
    double pidKp, double pidKd, double pidKi, int pidSampleTime,
    bool swapMotorPolarity, int feedforward,
    int brakeLowerThreshold, int brakeUpperThreshold,
    int minDuty, int maxDuty
) :
  motors(motors),
  motor(motors, motorPort),
  pid(
    angleSensor.state, pidKp, pidKd, pidKi,
    minDuty - feedforward, maxDuty - feedforward, pidSampleTime,
    minPosition, maxPosition
  ),
  speedAdjuster(pid.output, feedforward, brakeLowerThreshold, brakeUpperThreshold),
  swapMotorPolarity(swapMotorPolarity)
{
}

void CumulativeLinearActuator::setup() {
  motors.setup();
  if (swapMotorPolarity) motor.swapDirections();
  angleSensor.setup();
  pid.setup();
}

void CumulativeLinearActuator::update() {
  angleSensor.update();
  pid.update();
  speedAdjuster.update();
  motor.run(speedAdjuster.output.current());
}

}

