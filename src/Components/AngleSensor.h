#ifndef LPC_Components_AngleSensor_h
#define LPC_Components_AngleSensor_h

#include "StateVariable.h"

namespace LinearPositionControl { namespace Components {

class AS5601 {
  public:
    using Position = float;

    void setup();
    void update();

    bool updatedReading() const;
    uint16_t getReading() const;
    float getAngle() const;

  private:
    static const uint8_t deviceAddress = 0x36;
    enum class RegisterAddress : uint8_t {
      // Refer to https://www.mouser.com/ds/2/588/AS5601_DS000395_3-00-771178.pdf, page 19
      // Configuration Registers
      ZeroPosBurnCount = 0x00,
      ZeroPosHigh = 0x01,
      ZeroPosLow = 0x02,
      ConfHigh = 0x07,
      ConfLow = 0x08,
      QuadratureABN = 0x09,
      Pushthrough = 0x0A,
      // Output Registers
      RawAngleHigh = 0x0C,
      RawAngleLow = 0x0D,
      AngleHigh = 0x0E,
      AngleLow = 0x0F,
      // Status Registers
      Status = 0x0B,
      AutoGainControl = 0x1A,
      MagnitudeHigh = 0x1B,
      MagnitudeLow = 0x1C,
      // Burn Command
      Burn = 0xFF
    };

    bool setupCompleted = false;

    bool readingUpdated = false;
    uint16_t reading;

    void setRegisterAddressPointer(RegisterAddress addr);
    bool getByte(uint8_t &byte);
    void readAngle();
    float scaleReading(uint16_t reading) const;
};

class AngleSensor {
  public:
    // Note: currently magnetic3dSensorPort does nothing
    AngleSensor(
        uint8_t angleSensorPort, bool swapDirection = false,
        bool accumulate = true
    );

    using Position = float;

    void setup();
    void update();

    void setZero();
    void setReference(Position referencePosition);

    SimpleStateVariable<Position> state;

  private:
    bool setupCompleted = false;
    bool swapDirection;
    bool accumulate;

    uint8_t port;
    AS5601 sensor;
    SimpleStateVariable<Position> rawAngle;

    Position overflowDeltaThreshold = 315; // degrees

    void selectPort();
    void deselectPort();
};

} }

#include "AngleSensor.tpp"

#endif

