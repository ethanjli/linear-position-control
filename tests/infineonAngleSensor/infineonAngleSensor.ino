#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <MagneticSensor3D.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotor = AFMS.getMotor(3);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  magnetic3dSensor.begin();
  magnetic3dSensor.setAccessMode(magnetic3dSensor.MASTERCONTROLLEDMODE);
  magnetic3dSensor.disableTemp();
  AFMS.begin();  // create with the default frequency 1.6KHz
  myMotor->setSpeed(64);
  myMotor->run(FORWARD);
}

void loop() {
  delay(magnetic3dSensor.getMeasurementDelay());
  magnetic3dSensor.updateData();

  Serial.println(magnetic3dSensor.getAzimuth() * 180 / PI);
}

