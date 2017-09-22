#ifndef LinearActuator_h
#define LinearActuator_h

#include <elapsedMillis.h>

#include "DebouncedButton.h"
#include "Motors.h"

// A linear actuator with an absolute sense of direction - it knows which end is left and which end is right
class AbsoluteLinearActuator {
  public:
    AbsoluteLinearActuator(Motors *motors, MotorPort motorPort, DebouncedButton *leftLimit, DebouncedButton *rightLimit, bool debug_serial);

    void setup();
    void update();

    MotorSpeed speed = 255;

    void runForwards();
    void runBackwards();
    void brake();

  private:
    Motors *motors;
    const MotorPort motorPort;
    DebouncedButton *leftLimit;
    DebouncedButton *rightLimit;

    MotorDirection forwards = FORWARD;
    MotorDirection backwards = BACKWARD;

    int motorState;
    int limitsState;
    int previousLimitsState;
    int directionCalibrationState;
    void updateLimits();
    void updateUncalibrated();
    void updateCalibrated();
    void updateCalibrating();

    elapsedMillis motorStallTimer;
    unsigned int motorStallTimeout = 250;

    bool debug_serial = false;
};

#endif

