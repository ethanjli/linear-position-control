#ifndef Motion_SerialControl_h
#define Motion_SerialControl_h

#include "Motors.h"

namespace LinearPositionControl { namespace Motion {

template <class PositionTracker>
class SerialControl {
  public:
    SerialControl(Components::Motor &motor, PositionTracker &positionTracker);

    void setup();
    void update();
    void updateFromSerial();

    int speeds[35] = {
      0, 90, -90, 100, -100, 110, -110, 120, -120, 130, -130, 140, -140, 150, -150,
      160, -160, 170, -170, 180, -180, 190, -190, 200, -200, 210, -210, 220, -220,
      230, -230, 240, -240, 250, -250
    };

  private:
    bool setupCompleted = false;

    Components::Motor &motor;
    PositionTracker &positionTracker;
    int action = 0;
    int speed = 0;
};

} }

#include "SerialControl.tpp"

#endif

