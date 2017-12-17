# pipettor

These scripts provide user interfaces for high-level control of the pipettor module.

## Hardware and Firmware

These scripts are intended to run with a pipettor module running the pipettor sketch in `clients/pipettor` and connected to a computer over USB.

## Dependencies

All scripts are tested only on Python 3. To run any these scripts, the `pyserial` module must be installed.

## Modules and Scripts

### serialio

This module provides an `Arduino` class which makes it easy to communicate with an Arduino over USB using any ASCII-based protocol, and a `Console` class which provides similar functionality to the Arduino Serial Monitor. Running this module as a script (e.g. `python3 serialio.py`) will start a serial console; to quit the script, use `Ctrl+C`.

### pipettor

This module provides a `Pipettor` class which makes it easy to send position target commands to the pipettor and listen for events from the pipettor, namely events emitted when the module has initially reached and stabilized at its target position. This module also provides a `Targeting` interface class for easy use of a `Pipettor` object, and a `RandomTargeting` class which demonstrates how to implement the `Targeting` interface to listen for events.

The `RandomTargeting` class chooses random target positions; when the pipettor has reached the current target position, the `RandomTargeting` object will wait for 800 ms, choose a new random target position, and send it to the pipettor. This class can be tested by running this module as a script with `random` as the command line argument (e.g. `python3 pipettor.py random`); to quit the script, use `Ctrl+C`.

The `InteractiveTargeting` class interactively asks the user to choose target positions; when the pipettor has reached the current target position, the `InteractiveTargeting` object will prompt the user to enter a new random target position corresponding to a mL marking on the syringe (e.g. `0.6` or `0.6 mL` or `0.6mL`), and send it to the pipettor. This class can be tested by running this module as a script with `interactive` as the command line argument (e.g. `python3 pipettor.py interactive`); to quit the script, use `Ctrl+C`.

### pipettor_gui

This module provides a Tkinter graphical interface for the user to choose target positions. The interface features a **Target Position** slider to choose a target position for the pipettor; when the user has chosen a target position, they can press the **Go and Hold** button to send this new target position to the pipettor. Alternately, the user can toggle the **Follow Continuously** button; when button is this active, the pipettor moves in real time as the user drags the **Target Position** slider. The interface has a status bar at the bottom to show messages about what the pipettor is doing. This module should be run as a script (e.g. `python3 pipettor_gui.py`); to quit the script, close the window or press the **Quit** button.

### batch_demo

This module functions as an example of how to script a sequence of pipettor motions. This module will draw up 0.95 mL of fluid, wait for the user to press the *Enter* key, dispense 0.45 mL of fluid, wait for the user to press the *Enter* key, dispense 0.25 mL of fluid, wait for the user to press the *Enter* key, dispense the remaining fluid, and then quit. This module should be run as a script (e.g. `python3 batch_demo.py`); to quit the script, use `Ctrl+C`.

