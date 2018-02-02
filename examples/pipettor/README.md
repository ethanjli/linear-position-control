# pipettor

This Arduino sketch implements position control of the pipettor module and exposes a serial interface for specifying position setpoints.

## Hardware

This sketch controls the pipettor module subsystem. The OnShape CAD design for this subsystem can be found [here](https://cad.onshape.com/documents/6f3ff9e60612f07463807b51/w/7c9831bb106114d48918156b/e/1b8356a510263db4beefd007).  Laser cutting files can be found [here](https://drive.google.com/drive/u/0/folders/1LgJkTaF0e92JyGxylUDQz726a4ckO74z). Laser-cut pieces should be cut on 1/8 inch acrylic. Note that kerf adjustments are specific to the laser cutter in the Camarillo lab and may need to be adjusted for other laser cutters. The complete Bill of Materials for all non-acrylic components required to assemble this subsystem can be found [here](https://docs.google.com/spreadsheets/d/13kbot_s2HfTaFTPpONHiX8-gXCsIVLI1q1TLE1A9vvY/edit?usp=drive_web).

## Wiring

This sketch is configured to drive the motor on port M2 of the Adafruit Motor Shield. The red wire for the motor is assumed to be in the terminal block hole closer to GND on the Motor Shield than the blue wire for the motor. The red wire for the motor should be closer to the slide potentiometer than the blue wire. If the wires are switched, you will need to change the `motorReversedPolarity` constant to be `false`. A future iteration of the embedded software hardware abstraction layer will automatically detect motor polarity and swap motor directions when necessary.

The sketch uses pin A0 for output line of the slide potentiometer (pin 2 on the potentiometer); pin 1 of the potentiometer should connect to GND, while pin 3 of the potentiometer should connect to 5V. Pin 3 of the potentiometer should be closer to the syringe.

## Library Dependencies

To compile this sketch, the following libraries must be installed:

* Adafruit Motor Shield V2 Library
* Arduino Log
* Arduino PID Library
* elapsedMillis
* Linear Position Control

You should install these libraries from the Arduino IDE's Library Manager.

## Serial Protocol

This sketch supports easy usage over the Arduino Serial Monitor. More user-friendly control can be used with the various host programs in `hosts/pipettor`.

When the Arduino is first connected to a computer as a client over USB, it repeatedly sends the handshake character `~` until it receives a response (any response ending in a newline `\n` will do) from the host. The pipettor then moves to the highest possible position (configured by default in the sketch as `minPosition = 11`) and, when it has initially reached and stabilized at that position (defined as having stayed there for at least `brakethreshold = 100` milliseconds), reports its current position over serial in the format `[number]` (as its own line), for example `[8]`. At any time, the user can send a message in the format `[number]` to command the pipettor to move to the desired position. The pipettor will then move to the nearest position within the allowed range (set by the `minPosition` and `maxPosition` constants) and, when it has initially reached and stabilized at that position, report its current position over serial. The Arduino client will ignore any messages not of the format `[number]`.

## Control

Position control is done by PID control. Note that control parameters are tuned for operation with a monoject tuberculin syringe and may vary across motors and syringes. Control parameters have not yet been perfectly tuned. Note also that the potentiometer position readings at the limits of motion are hard-coded by the `minPosition` and `maxPosition` constants.

