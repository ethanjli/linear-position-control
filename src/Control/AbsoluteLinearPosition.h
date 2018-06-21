#ifndef LPC_Control_AbsoluteLinearPosition_h
#define LPC_Control_AbsoluteLinearPosition_h

#include "Components/Motors.h"
#include "Components/AnalogSensor.h"
#include "LinearPosition.h"

namespace LinearPositionControl { namespace Control {

using AbsoluteLinearActuator = LinearActuator<Components::AnalogSensor>;

} }

#endif

