#ifndef Tracking_Position_Position_h
#define Tracking_Position_Position_h

namespace LinearPositionControl { namespace Tracking {

namespace States {
  enum class Position : uint8_t {
    uncalibrated,
    unlocalized,
    localizing,
    tracking
  };
}

} }

#endif
