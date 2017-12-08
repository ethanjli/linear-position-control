#ifndef Motion_SerialControl_tpp
#define Motion_SerialControl_tpp

#include <ArduinoLog.h>

namespace LinearPositionControl { namespace Motion {

// SerialControl

template <class PositionTracker>
SerialControl<PositionTracker>::SerialControl(Components::Motor &motor, PositionTracker &positionTracker) :
  motor(motor), positionTracker(positionTracker) {
}

template <class PositionTracker>
void SerialControl<PositionTracker>::setup() {
  if (setupCompleted) return;

  motor.setup();
  positionTracker.setup();

  setupCompleted = true;
}

template <class PositionTracker>
void SerialControl<PositionTracker>::update() {
  positionTracker.update();
  if (positionTracker.state.current() != PositionTracker::State::tracking) return;
}

template <class PositionTracker>
void SerialControl<PositionTracker>::updateFromSerial() {
  while (Serial.available() <= 0) {;}
  action = Serial.read();
  speed = speeds[action];
  motor.speed = abs(speed);
  if (speed > 0) motor.forwards();
  else if (speed < 0) motor.backwards();
  else motor.brake(); // speed == 0
}

} }

#endif

