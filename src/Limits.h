#ifndef Limits_h
#define Limits_h

#include <elapsedMillis.h>

#include "StateVariable.h"
#include "DebouncedButton.h"
#include "Motors.h"

namespace LinearPositionControl { namespace Components {

namespace States {
  enum class Limits : uint8_t {
    none,
    left,
    right,
    either,
    both,
    unknown
  };
}

// Limit switches with an absolute sense of direction - the left and right switches are distinguishable and known
class AbsoluteLimits {
  public:
    AbsoluteLimits(DebouncedButton &leftLimit, DebouncedButton &rightLimit);

    using State = States::Limits;

    void setup();
    void update();

    StateVariable<State> state;

  private:
    bool setupCompleted = false;

    DebouncedButton &leftLimit;
    DebouncedButton &rightLimit;

    State currentState();
};

class MultiplexedLimits {
  public:
    MultiplexedLimits(DebouncedButton &leftAndRightLimits);

    using State = States::Limits;

    void setup();
    void update();

    StateVariable<State> state;

  private:
    bool setupCompleted = false;

    DebouncedButton &leftAndRightLimits;

    State currentState();
};

} }

#endif

