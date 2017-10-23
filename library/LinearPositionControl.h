#ifndef LinearPositionControl_h
#define LinearPositionControl_h

#include "StateVariable.h"
#include "Motors.h"
#include "LED.h"
#include "DebouncedButton.h"
#include "Limits.h"
#include "Calibration/Direction.h"
#include "Calibration/Position.h"
#include "LinearActuator.h"

namespace LinearPositionControl {

class SharedComponents {
  public:
    SharedComponents();

    Components::Motors motors;
    Components::LED led;

    void setup();
    void update();
};

template <
  class OpticalSensor,
  template <class, class> class PositionTrackingStrategy,
  template <class> class MotionControlStrategy
>
class UnmultiplexedLinearActuator {
  public:
    UnmultiplexedLinearActuator(
        SharedComponents &shared,
        MotorPort motorPort,
        uint8_t opticalSensorPin,
        volatile uint8_t &opticalSensorInterruptCounter,
        uint8_t limitSwitchLeftSensorPin,
        volatile uint8_t &limitSwitchLeftInterruptCounter,
        uint8_t limitSwitchRightSensorPin,
        volatile uint8_t &limitSwitchRightInterruptCounter
    );

    using Limits = Components::AbsoluteLimits;
    using DirectionCalibrator = Calibration::Direction<Limits>;
    using PositionTracker = PositionTrackingStrategy<Limits, OpticalSensor>;
    using PositionCalibrator = Calibration::Position<Limits, OpticalSensor, PositionTracker>;
    using MotionController = MotionControlStrategy<PositionTracker>;
    using Actuator = LinearActuator<
      DirectionCalibrator, PositionCalibrator, PositionTracker, MotionController
    >;
    using State = typename Actuator::State;

    SharedComponents &shared;
    Components::Motor motor;
    OpticalSensor opticalSensor;
    Components::DebouncedButton limitSwitchLeft;
    Components::DebouncedButton limitSwitchRight;
    Components::AbsoluteLimits limits;
    DirectionCalibrator directionCalibrator;
    PositionTracker positionTracker;
    PositionCalibrator positionCalibrator;
    MotionController motionController;
    Actuator actuator;
    StateVariable<State> &state;

    void setup();
    void update();
};

template <
  class OpticalSensor,
  template <class, class> class PositionTrackingStrategy,
  template <class> class MotionControlStrategy
>
class MultiplexedLinearActuator {
  public:
    MultiplexedLinearActuator(
        SharedComponents &shared,
        MotorPort motorPort,
        uint8_t opticalSensorPin,
        volatile uint8_t &opticalSensorInterruptCounter,
        uint8_t limitSwitchesSensorPin,
        volatile uint8_t &limitSwitchesInterruptCounter
    );

    using Limits = Components::MultiplexedLimits;
    using DirectionCalibrator = Calibration::Direction<Limits>;
    using PositionTracker = PositionTrackingStrategy<Limits, OpticalSensor>;
    using PositionCalibrator = Calibration::Position<Limits, OpticalSensor, PositionTracker>;
    using MotionController = MotionControlStrategy<PositionTracker>;
    using Actuator = LinearActuator<
      DirectionCalibrator, PositionCalibrator, PositionTracker, MotionController
    >;
    using State = typename Actuator::State;

    SharedComponents &shared;
    Components::Motor motor;
    OpticalSensor opticalSensor;
    Components::DebouncedButton limitSwitches;
    Components::MultiplexedLimits limits;
    DirectionCalibrator directionCalibrator;
    PositionTracker positionTracker;
    PositionCalibrator positionCalibrator;
    MotionController motionController;
    Actuator actuator;
    StateVariable<State> &state;

    void setup();
    void update();
};

}

#include "LinearPositionControl.tpp"

#endif

