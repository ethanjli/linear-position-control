#ifndef LPC_Control_LinearPosition_h
#define LPC_Control_LinearPosition_h

#include "Components/Motors.h"
#include "PID.h"

namespace LinearPositionControl { namespace Control {

template <class PositionSensor>
class LinearActuator {
  public:
    LinearActuator(
        Components::Motors &motors, MotorPort motorPort,
        uint8_t sensorId, int minPosition, int maxPosition, bool swapSensorDirection,
        double pidKp, double pidKd, double pidKi, int pidSampleTime,
        bool swapMotorPolarity, int feedforward,
        int brakeLowerThreshold, int brakeUpperThreshold,
        int minDuty = -255, int maxDuty = 255
    );

    using Position = typename PositionSensor::Position;

    Components::Motors &motors;
    Components::Motor motor;
    PositionSensor positionSensor;
    SimpleStateVariable<Position> &position;
    PIDController<Position, int> pid;
    Components::MotorSpeedAdjuster speedAdjuster;

    void setup();
    void update();

    void freeze(bool brake = true);
    void unfreeze();

  private:
    bool frozen;
};

} }

#include "LinearPosition.tpp"

#endif

