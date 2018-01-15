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
    uint8_t limitSwitchLeftSensorPin,
    uint8_t limitSwitchRightSensorPin
) :
  shared(shared),
  motor(shared.motors, motorPort),
  opticalSensor(opticalSensorPin, getInterruptCounter(opticalSensorPin)),
  limitSwitchLeft(
      limitSwitchLeftSensorPin, getInterruptCounter(limitSwitchLeftSensorPin), 5
  ),
  limitSwitchRight(
      limitSwitchRightSensorPin, getInterruptCounter(limitSwitchRightSensorPin), 5
  ),
  limits(limitSwitchLeft, limitSwitchRight),
  directionCalibrator(motor, limits),
  limitsTracker(motor, limits),
  positionTracker(motor, limitsTracker, opticalSensor),
  positionCalibrator(motor, limitsTracker, opticalSensor, positionTracker),
  motionController(motor, positionTracker),
  actuator(
    directionCalibrator, limitsTracker,
    positionCalibrator, positionTracker,
    motionController
  ),
  state(actuator.state) {
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
    uint8_t limitSwitchesSensorPin
) :
  shared(shared),
  motor(shared.motors, motorPort),
  opticalSensor(opticalSensorPin, getInterruptCounter(opticalSensorPin)),
  limitSwitches(limitSwitchesSensorPin, getInterruptCounter(limitSwitchesSensorPin), 5),
  limits(limitSwitches),
  directionCalibrator(motor, limits),
  limitsTracker(motor, limits),
  positionTracker(motor, limitsTracker, opticalSensor),
  positionCalibrator(motor, limitsTracker, opticalSensor, positionTracker),
  motionController(motor, positionTracker),
  actuator(
    directionCalibrator, limitsTracker,
    positionCalibrator, positionTracker,
    motionController
  ),
  state(actuator.state)
{
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

// AbsoluteLinearActuator

AbsoluteLinearActuator::AbsoluteLinearActuator(
    SharedComponents &shared, MotorPort motorPort,
    uint8_t potentiometerPin, int minPosition, int maxPosition,
    double pidKp, double pidKd, double pidKi, int pidSampleTime,
    bool swapMotorPolarity, int feedforward, int brakeThreshold
) :
  shared(shared),
  motor(shared.motors, motorPort),
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
  shared.motors.setup();
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

// CalibrationRig

template<class EncapsulatedLinearActuator>
CalibrationRig<EncapsulatedLinearActuator>::CalibrationRig(
    SharedComponents &shared,
    EncapsulatedLinearActuator &actuator,
    uint8_t potentiometerPin
) :
  shared(shared),
  actuator(actuator),
  potentiometer(potentiometerPin),
  state(actuator.state) {
}

template<class EncapsulatedLinearActuator>
void CalibrationRig<EncapsulatedLinearActuator>::setup() {
  Log.begin(LOG_LEVEL_ERROR, &Serial);
  Serial.begin(115200);
  actuator.setup();
  potentiometer.setup();
}

template<class EncapsulatedLinearActuator>
void CalibrationRig<EncapsulatedLinearActuator>::update() {
  actuator.update();
  potentiometer.update();
}

template<class EncapsulatedLinearActuator>
int CalibrationRig<EncapsulatedLinearActuator>::mapToEdgeCount(int position) const {
  return actuator.positionTracker.mapPositionFrom(position, minPosition, maxPosition);
}

template<class EncapsulatedLinearActuator>
int CalibrationRig<EncapsulatedLinearActuator>::mapToPosition(int edgeCount) const {
  return actuator.positionTracker.mapPositionTo(edgeCount, minPosition, maxPosition);
}

template<class EncapsulatedLinearActuator>
void CalibrationRig<EncapsulatedLinearActuator>::startNewEpisode() {
  targetPosition = random(minPosition, maxPosition + 1);
  actuator.motionController.targetPosition.update(mapToEdgeCount(targetPosition));
  targetingTimer = 0;
  targetingTimerMicroseconds = 0;
  ++episodeID;
  targeting = true;
}

template<class EncapsulatedLinearActuator>
void CalibrationRig<EncapsulatedLinearActuator>::startPreEpisodeLocalization() {
  targetPosition = random(2) ? minPosition : maxPosition;
  actuator.motionController.targetPosition.update(mapToEdgeCount(targetPosition));
  targeting = false;
}

template<class EncapsulatedLinearActuator>
void CalibrationRig<EncapsulatedLinearActuator>::waitForSerialHandshake() const {
  while (!Serial) {;}
  while (Serial.available() < 1) {
    Serial.println('~');
    delay(200);
  }
  Serial.read();
  delay(200);
}

template<class EncapsulatedLinearActuator>
void CalibrationRig<EncapsulatedLinearActuator>::printHeader() const {
  Serial.print('[');
  // ID
  Serial.print(F("episodeID,"));
  // Clock
  //Serial.print(F("targetingTimeMilliseconds,"));
  Serial.print(F("targetingTimeMicroseconds,"));
  // Target
  Serial.print(F("targetPosition,"));
  //Serial.print(F("targetEdgesFromLeft,"));
  // Ground truth
  Serial.print(F("groundTruthPosition,"));
  // Baseline prediction
  Serial.print(F("estimatedPosition,"));
  //Serial.print(F("estimatedEdgesFromLeft,"));
  // Limits features
  Serial.print(F("lastLimit,"));
  //Serial.print(F("timeSinceLastLimit,"));
  Serial.print(F("timeSinceLastLimitMicroseconds,"));
  Serial.print(F("edgesBetweenLimits,"));
  Serial.print(F("atLeftLimit,"));
  Serial.print(F("atRightLimit,"));
  // Position tracking features
  //Serial.print(F("timeSinceLastEdge,"));
  Serial.print(F("timeSinceLastEdgeMicroseconds,"));
  //Serial.print(F("timeBetweenLastEdges,"));
  Serial.print(F("timeBetweenLastEdgesMicroseconds,"));
  Serial.print(F("forwardsEdgesSinceLastLimit,"));
  Serial.print(F("backwardsEdgesSinceLastLimit,"));
  // Optical sensor features
  Serial.print(F("opticalSensor,"));
  // Motor features
  Serial.print(F("motorDirection,"));
  Serial.print(F("motorDuty"));

  Serial.println(']');
}

template<class EncapsulatedLinearActuator>
void CalibrationRig<EncapsulatedLinearActuator>::printState(bool targetingOnly) const {
  if (targetingOnly && targetPosition == -1) return;

  Serial.print('[');
  // ID
  Serial.print(episodeID);
  Serial.print(',');
  // Clock
  //Serial.print(targetingTimer);
  //Serial.print(',');
  Serial.print(targetingTimerMicroseconds);
  Serial.print(',');
  // Target
  Serial.print(targetPosition);
  Serial.print(',');
  //Serial.print(mapToEdgeCount(targetPosition));
  //Serial.print(',');
  // Ground truth
  Serial.print(potentiometer.state.current());
  Serial.print(',');
  // Baseline prediction
  Serial.print(mapToPosition(actuator.positionTracker.position.current()));
  Serial.print(',');
  //Serial.print(actuator.positionTracker.position.current());
  //Serial.print(',');
  // Limits features
  switch (actuator.limitsTracker.lastLimit.current()) {
    case Components::States::Limits::left:
      Serial.print(-1);
      break;
    case Components::States::Limits::right:
      Serial.print(1);
      break;
  }
  Serial.print(',');
  //Serial.print(actuator.limitsTracker.lastLimit.currentDuration());
  //Serial.print(',');
  Serial.print(actuator.limitsTracker.lastLimit.currentDurationMicros());
  Serial.print(',');
  Serial.print(actuator.positionTracker.getNumTotalEdges());
  Serial.print(',');
  Serial.print(actuator.positionTracker.atLeftLimit());
  Serial.print(',');
  Serial.print(actuator.positionTracker.atRightLimit());
  Serial.print(',');
  // Position tracking features
  //Serial.print(actuator.positionTracker.position.currentDuration());
  //Serial.print(',');
  Serial.print(actuator.positionTracker.position.currentDurationMicros());
  Serial.print(',');
  //Serial.print(actuator.positionTracker.position.previousDistinctDuration());
  //Serial.print(',');
  Serial.print(actuator.positionTracker.position.previousDistinctDurationMicros());
  Serial.print(',');
  Serial.print(actuator.positionTracker.forwardsEdgesSinceLastLimit);
  Serial.print(',');
  Serial.print(actuator.positionTracker.backwardsEdgesSinceLastLimit);
  Serial.print(',');
  // Optical sensor features
  switch (actuator.opticalSensor.state.current()) {
    case Components::States::EdgeCounter::light:
      Serial.print(1);
      break;
    case Components::States::EdgeCounter::dark:
      Serial.print(-1);
      break;
    default:
      Serial.print(0);
  }
  Serial.print(',');
  // Motor features
  switch (actuator.motor.state.current()) {
    case Components::States::Motor::forwards:
      Serial.print(1);
      break;
    case Components::States::Motor::backwards:
      Serial.print(-1);
      break;
    default:
      Serial.print(0);
  }
  Serial.print(',');
  Serial.print(actuator.motor.speed);

  Serial.println(']');
}

}

#endif

