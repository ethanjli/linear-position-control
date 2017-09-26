#ifndef LinearActuator_h
#define LinearActuator_h

#include <elapsedMillis.h>

#include "DebouncedButton.h"
#include "Motors.h"

// A linear actuator with an absolute sense of direction - it knows which end is left and which end is right
template <bool debug_serial>
class AbsoluteLinearActuator {
  public:
    AbsoluteLinearActuator(Motors *motors, MotorPort motorPort, DebouncedButton *leftLimit, DebouncedButton *rightLimit);

    void setup();
    void update();

    // Actuation
    MotorSpeed speed = 255;
    void runForwards();
    void runBackwards();
    void brake();

  private:
    // Actuation
    Motors *motors;
    const MotorPort motorPort;
    MotorDirection forwards = FORWARD;
    MotorDirection backwards = BACKWARD;

    // Sensing
    DebouncedButton *leftLimit;
    DebouncedButton *rightLimit;
    void updateLimits();

    // States
    int motorState;
    int limitsState;
    int previousLimitsState;
    int directionCalibrationState;
    int positionCalibrationState;

    // Calibration
    elapsedMillis motorStallTimer;
    unsigned int motorStallTimeout = 250;
    // Direction Calibration
    void updateDirectionUncalibrated();
    void updateDirectionCalibrated();
    void updateDirectionCalibrating();
    // Position Calibration
    void updatePositionUncalibrated();
    void updatePositionCalibrating();
    void updatePositionCalibrated();
};

#include "LinearActuator.tpp"

#endif

