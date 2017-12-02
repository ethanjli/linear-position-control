#ifndef LinearPositionControl_h
#define LinearPositionControl_h

#include <elapsedMillis.h>

#include "FastInterrupts.h"
#include "StateVariable.h"
#include "Motors.h"
#include "LED.h"
#include "DebouncedButton.h"
#include "Limits.h"
#include "AnalogSensor.h"
#include "Calibration/Direction.h"
#include "Calibration/Position.h"
#include "Tracking/Limits.h"
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
        uint8_t limitSwitchLeftSensorPin,
        uint8_t limitSwitchRightSensorPin
    );

    using Limits = Components::AbsoluteLimits;
    using DirectionCalibrator = Calibration::Direction<Limits>;
    using LimitsTracker = Tracking::AbsoluteLimits<Limits>;
    using PositionTracker = PositionTrackingStrategy<Limits, OpticalSensor>;
    using PositionCalibrator = Calibration::Position<Limits, OpticalSensor, PositionTracker>;
    using MotionController = MotionControlStrategy<PositionTracker>;
    using Actuator = LinearActuator<
      DirectionCalibrator, LimitsTracker,
      PositionCalibrator, PositionTracker,
      MotionController
    >;
    using State = typename Actuator::State;

    SharedComponents &shared;
    Components::Motor motor;
    OpticalSensor opticalSensor;
    Components::DebouncedButton limitSwitchLeft;
    Components::DebouncedButton limitSwitchRight;
    Components::AbsoluteLimits limits;
    DirectionCalibrator directionCalibrator;
    LimitsTracker limitsTracker;
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
        uint8_t limitSwitchesSensorPin
    );

    using Limits = Components::MultiplexedLimits;
    using DirectionCalibrator = Calibration::Direction<Limits>;
    using LimitsTracker = Tracking::AbsoluteLimits<Limits>;
    using PositionTracker = PositionTrackingStrategy<Limits, OpticalSensor>;
    using PositionCalibrator = Calibration::Position<Limits, OpticalSensor, PositionTracker>;
    using MotionController = MotionControlStrategy<PositionTracker>;
    using Actuator = LinearActuator<
      DirectionCalibrator, LimitsTracker,
      PositionCalibrator, PositionTracker,
      MotionController
    >;
    using State = typename Actuator::State;

    SharedComponents &shared;
    Components::Motor motor;
    OpticalSensor opticalSensor;
    Components::DebouncedButton limitSwitches;
    Components::MultiplexedLimits limits;
    DirectionCalibrator directionCalibrator;
    LimitsTracker limitsTracker;
    PositionTracker positionTracker;
    PositionCalibrator positionCalibrator;
    MotionController motionController;
    Actuator actuator;
    StateVariable<State> &state;

    void setup();
    void update();
};

template<class EncapsulatedLinearActuator>
class CalibrationRig {
  public:
    CalibrationRig(
        SharedComponents &shared,
        EncapsulatedLinearActuator &actuator,
        uint8_t potentiometerPin
    );

    using State = typename EncapsulatedLinearActuator::State;

    SharedComponents &shared;
    EncapsulatedLinearActuator &actuator;
    Components::AnalogSensor potentiometer;
    StateVariable<State> &state;

    const int minPosition = 37;
    const int maxPosition = 992;

    int targetPosition = -1;
    elapsedMillis targetingTimer;
    elapsedMicros targetingTimerMicroseconds;
    int episodeID = -1;
    bool targeting = true;

    void setup();
    void update();

    int mapToEdgeCount(int position) const;
    int mapToPosition(int edgeCount) const;

    void startNewEpisode();
    void startPreEpisodeLocalization();
    void printHeader() const;
    void printState() const;
};

}

#include "LinearPositionControl.tpp"

#endif

