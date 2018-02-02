# linear-position-control
Hardware abstraction layer to enable closed-loop position control of linear actuators with low-cost components.

## Dependencies

All sketches require the following dependencies, which should be installed by the Arduino IDE's Library Manager:

* [Adafruit Motor Shield V2 Library](https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library)
* [ArduinoLog](https://github.com/thijse/Arduino-Log/)
* [elapsedMillis](https://github.com/pfeerick/elapsedMillis/wiki)

Additional dependencies must be installed for specific functionalities:

* [Arduino PID Library](http://playground.arduino.cc/Code/PIDLibrary) for PID-based controllers. This should be installed through the Arduino IDE. You probably need this.
* [EnableInterrupt](https://github.com/GreyGnome/EnableInterrupt) for sensors requiring switch debouncing or edge detection. This should be installed through the Arduino IDE. You need this for actuators which don't use a potentiometer for position sensing.
* [LEDFader](https://github.com/jgillick/arduino-LEDFader) for preset LED fade patterns (useful in indicating actuator status). This must be installed manually following the instructions in the link, as it's registered in the Arduino IDE Library Manager. You probably don't need this, and in the future this will be replaced with a library available through the Arduino IDE Library Manager.

