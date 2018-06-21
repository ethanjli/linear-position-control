#ifndef AbsoluteLinearPositionControl_h
#define AbsoluteLinearPositionControl_h

#include "Components/Motors.h"
#include "Components/AnalogSensor.h"
#include "LinearPositionControl.h"

namespace LinearPositionControl {

using AbsoluteLinearActuator = LinearActuator<Components::AnalogSensor>;

}

#endif

