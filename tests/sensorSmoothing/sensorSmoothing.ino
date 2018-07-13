#define LPC_Components_AnalogSensor
#define LPC_Signal_Smoothing
#include <LinearPositionControl.h>

using namespace LinearPositionControl;

Components::AnalogSensor potentiometer(A0);
Signal::DiscreteSmoother smoother(potentiometer.state, 0.01, 1023, true, 2.0);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  potentiometer.setup();
  smoother.setup();
}

void loop() {
  potentiometer.update();
  smoother.update();
  Serial.print("0,1023,");
  Serial.print(potentiometer.state.current);
  Serial.print(',');
  Serial.print(smoother.output.current);
  Serial.println();
}

