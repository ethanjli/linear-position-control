#ifndef Limits_h
#define Limits_h

#include <elapsedMillis.h>

#include "DebouncedButton.h"
#include "Motors.h"

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
template <bool debug_serial>
class AbsoluteLimits {
  public:
    AbsoluteLimits(DebouncedButton *leftLimit, DebouncedButton *rightLimit);

    void setup();
    void update();

    States::Limits state;
    States::Limits previousState;

  private:
    DebouncedButton *leftLimit;
    DebouncedButton *rightLimit;
};

template <bool debug_serial>
class MultiplexedLimits {
};

#include "Limits.tpp"

#endif

