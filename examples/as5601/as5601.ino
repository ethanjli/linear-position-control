#include <Wire.h>

// Read angle from as5601
//
// AS5601: 
//  1 VDD5V (0.1uF to ground; +5V)   8 A   
//  2 VDD3V3 (1uF to ground)         7 SCL
//  3 PUSH                           6 SDA
//  4 GND                            5 B
///////////////////////////////////////////////

const uint8_t deviceAddress = 0x36;
const uint8_t reg_angleHigh = 0x0C;
const uint8_t reg_angleLow = 0x0D;
uint16_t reading;
uint16_t prevReading;
float angle = 0;
float prevAngle = 0;
float delta = 0;
float accumulatedAngle = 0;
const float overflowDeltaThreshold = 315;
const uint16_t lowRegisterOverflow = 255;
const float lowRegisterFloatOverflow = lowRegisterOverflow / 4095.0 * 360;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  setReg(reg_angleHigh); // If we never set the register to anything else, we don't have to re-set it
  while (!readAngle(reading)) continue;
  angle = scaleReading(reading);
}

float scaleReading(uint16_t reading) {
// convert 12 bit angle reading to decimal
  return (reading / 4095.0) * 360;
}

void setReg(uint8_t reg) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(reg);
  Wire.endTransmission();
}

int getByte() {
  Wire.requestFrom(deviceAddress, (uint8_t) 1);
  if (1 <= Wire.available()) return 0xFF & Wire.read();
  else return -1;
}

bool readAngle(uint16_t &reading) {
  int readingHigh = getByte();
  if (0 <= readingHigh) {
    int readingLow = getByte();
    if (0 <= readingLow) {
      reading = (readingHigh << 8) | readingLow;
      return true;
    }
  }
  return false;
}

void loop() {
  uint16_t nextReading;
  if (readAngle(nextReading)) {
    prevReading = reading;
    reading = nextReading;
    prevAngle = angle;
    angle = scaleReading(reading);
    delta = angle - prevAngle;
    if (abs(delta) > overflowDeltaThreshold) {
      int deltaSign = (0.0f < delta) - (delta < 0.0f);
      delta -= deltaSign * 360.0f;
    }/* else if ((delta > 0) && (abs(delta - lowRegisterFloatOverflow) < abs(delta))) {
      delta -= lowRegisterFloatOverflow;
    } else if ((delta < 0) && (abs(delta + lowRegisterFloatOverflow) < abs(delta))) {
      delta += lowRegisterFloatOverflow;
    }*/
    accumulatedAngle += delta;
    Serial.println(accumulatedAngle);
  }
}
