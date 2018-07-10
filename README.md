# linear-position-control
Hardware abstraction layer to enable closed-loop position control of linear actuators with low-cost components.

## Dependencies

The library requires the following core dependencies, each of which should be installed through the Arduino IDE's Library Manager:

* [Adafruit Motor Shield V2 Library](https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library) to drive motors.
* [elapsedMillis](https://github.com/pfeerick/elapsedMillis/wiki) for timing.
* [ArduinoLog](https://github.com/thijse/Arduino-Log/) for error logging and troubleshooting over serial.
* [Arduino PID Library](http://playground.arduino.cc/Code/PIDLibrary) for PID-based controllers.

Additional dependencies must be installed for specific functionalities:

* [ResponsiveAnalogRead](https://github.com/dxinteractive/ResponsiveAnalogRead) for signal smoothing. This should be installed through the Arduino IDE.
* [3D-Magnetic-Sensor-2GO](https://github.com/Infineon/TLV493D-A1B6-3DMagnetic-Sensor) for use of angle sensors in cumulative linear actuators. This should be installed through the Arduino IDE. In the future this dependency will be removed.
* [EnableInterrupt](https://github.com/GreyGnome/EnableInterrupt) for sensors requiring button/switch debouncing. This should be installed through the Arduino IDE.
* [LEDFader](https://github.com/jgillick/arduino-LEDFader) for preset LED fade patterns (useful in indicating actuator status). This must be installed manually following the instructions in the link, as it's not registered in the Arduino IDE Library Manager. You probably don't need this, and in the future this will be replaced with a library available through the Arduino IDE Library Manager.

## Installation

After installing the dependencies, this library should also be installed through the Arduino IDE's Library Manager. It's named "Linear Position Control".

## Getting Started

Please follow the instructions in the "Getting Started" section of the documentation for the [Liquid Handling Robotics project](http://liquid-handling-robotics.readthedocs.io/en/latest/index.html).

