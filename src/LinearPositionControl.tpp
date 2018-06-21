#ifndef LinearPositionControl_tpp
#define LinearPositionControl_tpp

namespace LinearPositionControl {

// LinearActuator

template <class PositionSensor>
LinearActuator<PositionSensor>::LinearActuator(
    Components::Motors &motors, MotorPort motorPort,
    uint8_t sensorId, int minPosition, int maxPosition,
    double pidKp, double pidKd, double pidKi, int pidSampleTime,
    bool swapMotorPolarity, int feedforward,
    int brakeLowerThreshold, int brakeUpperThreshold,
    int minDuty, int maxDuty
) :
  motors(motors), motor(motors, motorPort),
  positionSensor(sensorId), position(positionSensor.state),
  pid(
    position.current, pidKp, pidKd, pidKi,
    minDuty - feedforward, maxDuty - feedforward, pidSampleTime,
    minPosition, maxPosition
  ),
  speedAdjuster(pid.output, feedforward, brakeLowerThreshold, brakeUpperThreshold),
  swapMotorPolarity(swapMotorPolarity)
{
}

template <class PositionSensor>
void LinearActuator<PositionSensor>::setup() {
  motors.setup();
  if (swapMotorPolarity) motor.swapDirections();
  positionSensor.setup();
  positionSensor.setZero();
  pid.setup();
}

template <class PositionSensor>
void LinearActuator<PositionSensor>::update() {
  positionSensor.update();
  pid.update();
  speedAdjuster.update();
  if (!frozen) motor.run(speedAdjuster.output.current);
}

template <class PositionSensor>
void LinearActuator<PositionSensor>::freeze(bool brake) {
  pid.disable();
  speedAdjuster.freeze();
  if (brake) motor.brake();
  frozen = true;
}

template <class PositionSensor>
void LinearActuator<PositionSensor>::unfreeze() {
  pid.enable();
  speedAdjuster.unfreeze();
  frozen = false;
}

}

#endif

