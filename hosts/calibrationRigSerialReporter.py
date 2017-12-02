#!/usr/bin/env python3
import time
import sys

import serial

FILENAME = 'calibrationRig.csv'

def get_episode_id(line):
    episode_id = line[1:].split(',')[0]
    if episode_id == 'episodeID':
        return None
    else:
        return int(episode_id)

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
        current_episode = None
        while True:
            line = str(ser.readline(), 'ascii').rstrip()
            num_fields = line.count(',')
            if num_fields_expected is None:
                num_fields_expected = num_fields
                print('Number of fields in output:', num_fields_expected)
            if line.startswith('[') and line.endswith(']') and num_fields == num_fields_expected:
                print(line[1:-1], file=f)
                f.close()
                new_episode = get_episode_id(line)
                if (new_episode != current_episode) and (new_episode is not None):
                    current_episode = new_episode
                    if current_episode % 10 == 0:
                        print('Starting episode {}...'.format(current_episode))
                f = open('calibrationRig.csv', 'a')
            elif current_episode is not None and line.startswith('E: '):
                print('Error on episode {}: {}'.format(current_episode, line[2:]))
            elif current_episode is not None and line.startswith('W: '):
                print('Warning on episode {}: {}'.format(current_episode, line[2:]))
            elif current_episode is None:
                print('Initialization message: {}'.format(line))
            else:
                print('Malformed line:', line)
    except KeyboardInterrupt:
        print('Quitting...')
        f.close()


if __name__ == '__main__':
    main()

