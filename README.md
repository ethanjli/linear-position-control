# linear-position-control
Hardware abstraction layer to enable closed-loop position control of linear actuators with low-cost components.

Caution: This package is still in pre-release mode; all interfaces are guaranteed to change, functionality is not yet complete, and support is not yet provided. Because it is needed for user testing for a pilot study, we are creating releases for the Arduino IDE's Library Manager to enable deployment via the Arduino Web Editor.

## Dependencies

The library requires the following core dependencies, each of which should be installed through the Arduino IDE's Library Manager:

* [Adafruit Motor Shield V2 Library](https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library) to drive motors. In the future, libraries for alternative boards may be supported if there is demand.
* [elapsedMillis](https://github.com/pfeerick/elapsedMillis/wiki) for timing.
* [ArduinoLog](https://github.com/thijse/Arduino-Log/) for error logging and troubleshooting over serial.
* [Arduino PID Library](http://playground.arduino.cc/Code/PIDLibrary) for PID-based controllers.

Additional dependencies must be installed for specific functionalities:

* [EnableInterrupt](https://github.com/GreyGnome/EnableInterrupt) for sensors requiring switch debouncing or edge detection. This should be installed through the Arduino IDE. You need this for actuators which don't use a potentiometer for position sensing.
* [LEDFader](https://github.com/jgillick/arduino-LEDFader) for preset LED fade patterns (useful in indicating actuator status). This must be installed manually following the instructions in the link, as it's registered in the Arduino IDE Library Manager. You probably don't need this, and in the future this will be replaced with a library available through the Arduino IDE Library Manager.

## Installation

After installing the dependencies, this library should also be installed through the Arduino IDE's Library Manager. It's named "Linear Position Control".

## Getting Started

Please follow the instructions in the "Getting Started" section of the documentation for the [Liquid Handling Robotics project](http://liquid-handling-robotics.readthedocs.io/en/latest/index.html).

