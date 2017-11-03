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
        num_fields_expected = None
        while True:
            line = str(ser.readline(), 'ascii').rstrip()
            num_fields = line.count(',')
            if num_fields_expected is None:
                num_fields_expected = num_fields
                print('Number of fields in output:', num_fields_expected)
            if not (line.startswith('[') and line.endswith(']') and
                    num_fields == num_fields_expected):
                print('Malformed line:', line)
            else:
                print(line[1:-1], file=f)
                f.close()
                f = open('calibrationRig.csv', 'a')
    except KeyboardInterrupt:
        print('Quitting...')
        f.close()


if __name__ == '__main__':
    main()

