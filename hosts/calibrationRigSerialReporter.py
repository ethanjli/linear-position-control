#!/usr/bin/env python3
import time
import sys

import serial

FILENAME = 'calibrationRig.csv'

def main():
    print('Please connect the calibration rig now...')
    time.sleep(2.0)
    while True:
        try:
            ser = serial.Serial('/dev/ttyACM0', 115200)
            break
        except serial.serialutil.SerialException:
            print('Arduino not found. Trying again in 1 second...')
            time.sleep(1.0)

    print('Connected!')

    f = open('calibrationRig.csv', 'w')

    try:
        while True:
            line = str(ser.readline(), 'ascii')
            sys.stdout.write(line)
            f.write(line)
            f.close()
            f = open('calibrationRig.csv', 'a')
    except KeyboardInterrupt:
        print('Quitting...')
        f.close()


if __name__ == '__main__':
    main()

