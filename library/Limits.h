#ifndef Limits_h
#define Limits_h

#include <elapsedMillis.h>

#include "DebouncedButton.h"
#include "Motors.h"

namespace LinearPositionControl { namespace Components {

namespace States {
  enum class Limits : uint8_t {
    none,
    left,
    right,
    either,
    both
  };
}

// Limit switches with an absolute sense of direction - the left and right switches are distinguishable and known
class AbsoluteLimits {
  public:
    AbsoluteLimits(DebouncedButton &leftLimit, DebouncedButton &rightLimit);

    using States = States::Limits;

    void setup();
    void update();

    States state;
    States previousState;

  private:
    bool setupCompleted = false;

    DebouncedButton &leftLimit;
    DebouncedButton &rightLimit;
};

class MultiplexedLimits {
  public:
    MultiplexedLimits(DebouncedButton &leftAndRightLimits);

    using States = States::Limits;

    void setup();
    void update();

    States state;
    States previousState;

  private:
    bool setupCompleted = false;

    DebouncedButton &leftAndRightLimits;
};

} }

#endif

