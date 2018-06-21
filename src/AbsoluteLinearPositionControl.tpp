#ifndef AbsoluteLinearPositionControl_tpp
#define AbsoluteLinearPositionControl_tpp

namespace LinearPositionControl {

// AbsoluteLinearActuator

AbsoluteLinearActuator::AbsoluteLinearActuator(
    Components::Motors &motors, MotorPort motorPort,
    uint8_t potentiometerPin, int minPosition, int maxPosition,
    double pidKp, double pidKd, double pidKi, int pidSampleTime,
    bool swapMotorPolarity, int feedforward,
    int brakeLowerThreshold, int brakeUpperThreshold,
    int minDuty, int maxDuty
) :
  motors(motors),
  motor(motors, motorPort),
  potentiometer(potentiometerPin),
  position(potentiometer.state),
  pid(
    position.current, pidKp, pidKd, pidKi,
    minDuty - feedforward, maxDuty - feedforward, pidSampleTime,
    minPosition, maxPosition
  ),
  speedAdjuster(pid.output, feedforward, brakeLowerThreshold, brakeUpperThreshold),
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
  if (!frozen) motor.run(speedAdjuster.output.current);
}

void AbsoluteLinearActuator::freeze(bool brake) {
  pid.disable();
  speedAdjuster.freeze();
  if (brake) motor.brake();
  frozen = true;
}

void AbsoluteLinearActuator::unfreeze() {
  pid.enable();
  speedAdjuster.unfreeze();
  frozen = false;
}

}

#endif

