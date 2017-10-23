#ifndef LinearPositionControl_tpp
#define LinearPositionControl_tpp

namespace LinearPositionControl {

// MultiplexedSharedComponents
SharedComponents::SharedComponents() :
  led(Components::LED(LED_BUILTIN)) {
}

void SharedComponents::setup() {
  led.setup();
}

void SharedComponents::update() {
  led.update();
}

// UnmultiplexedLinearActuator

template <
  class OpticalSensor,
  template <class, class> class PositionTrackingStrategy,
  template <class> class MotionControlStrategy
>
UnmultiplexedLinearActuator<
  OpticalSensor,
  PositionTrackingStrategy,
  MotionControlStrategy
>::UnmultiplexedLinearActuator(
    SharedComponents &shared,
    MotorPort motorPort,
    uint8_t opticalSensorPin,
    volatile uint8_t &opticalSensorInterruptCounter,
    uint8_t limitSwitchLeftSensorPin,
    volatile uint8_t &limitSwitchLeftInterruptCounter,
    uint8_t limitSwitchRightSensorPin,
    volatile uint8_t &limitSwitchRightInterruptCounter
) :
  shared(shared),
  motor(shared.motors, motorPort),
  opticalSensor(opticalSensorPin, opticalSensorInterruptCounter),
  limitSwitchLeft(limitSwitchLeftSensorPin, limitSwitchLeftInterruptCounter, 5),
  limitSwitchRight(limitSwitchRightSensorPin, limitSwitchRightInterruptCounter, 5),
  limits(limitSwitchLeft, limitSwitchRight),
  directionCalibrator(motor, limits),
  positionTracker(motor, limits, opticalSensor),
  positionCalibrator(motor, limits, opticalSensor, positionTracker),
  motionController(motor, positionTracker),
  actuator(directionCalibrator, positionCalibrator, positionTracker, motionController) {
}

template <
  class OpticalSensor,
  template <class, class> class PositionTrackingStrategy,
  template <class> class MotionControlStrategy
>
void UnmultiplexedLinearActuator<
  OpticalSensor,
  PositionTrackingStrategy,
  MotionControlStrategy
>::setup() {
  shared.setup();
  actuator.setup();
}

template <
  class OpticalSensor,
  template <class, class> class PositionTrackingStrategy,
  template <class> class MotionControlStrategy
>
void UnmultiplexedLinearActuator<
  OpticalSensor,
  PositionTrackingStrategy,
  MotionControlStrategy
>::update() {
  shared.update();
  actuator.update();
}

// MultiplexedLinearActuator

template <
  class OpticalSensor,
  template <class, class> class PositionTrackingStrategy,
  template <class> class MotionControlStrategy
>
MultiplexedLinearActuator<
  OpticalSensor,
  PositionTrackingStrategy,
  MotionControlStrategy
>::MultiplexedLinearActuator(
    SharedComponents &shared,
    MotorPort motorPort,
    uint8_t opticalSensorPin,
    volatile uint8_t &opticalSensorInterruptCounter,
    uint8_t limitSwitchesSensorPin,
    volatile uint8_t &limitSwitchesInterruptCounter
) :
  shared(shared),
  motor(shared.motors, motorPort),
  opticalSensor(opticalSensorPin, opticalSensorInterruptCounter),
  limitSwitches(limitSwitchesSensorPin, limitSwitchesInterruptCounter, 5),
  limits(limitSwitches),
  directionCalibrator(motor, limits),
  positionTracker(motor, limits, opticalSensor),
  positionCalibrator(motor, limits, opticalSensor, positionTracker),
  motionController(motor, positionTracker),
  actuator(directionCalibrator, positionCalibrator, positionTracker, motionController) {
}

template <
  class OpticalSensor,
  template <class, class> class PositionTrackingStrategy,
  template <class> class MotionControlStrategy
>
void MultiplexedLinearActuator<
  OpticalSensor,
  PositionTrackingStrategy,
  MotionControlStrategy
>::setup() {
  shared.setup();
  actuator.setup();
}

template <
  class OpticalSensor,
  template <class, class> class PositionTrackingStrategy,
  template <class> class MotionControlStrategy
>
void MultiplexedLinearActuator<
  OpticalSensor,
  PositionTrackingStrategy,
  MotionControlStrategy
>::update() {
  shared.update();
  actuator.update();
}

}

#endif

