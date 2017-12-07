#!/usr/bin/env python3
import time
import sys

import serial

FEATURES_FILENAME = 'calibrationRig.csv'
EXCLUSIONS_FILENAME = 'exclusions.txt'
#NUM_EPISODES = 24000 # traindev
NUM_EPISODES = 4000 # test

class SerialLineStream():
    def __init__(self):
        self.ser = None

    def connect(self, port='/dev/ttyACM0', baudrate=115200):
        print('Please connect the device now...')
        time.sleep(2.0)
        while True:
            try:
                self.ser = serial.Serial(port, baudrate)
                break
            except serial.serialutil.SerialException:
                time.sleep(0.2)
        print('Connected!')

    def get_line(self):
        return str(self.ser.readline(), 'ascii').rstrip()

    def write_byte(self, byte, length=1, byte_order='big', signed=False):
        self.ser.write(byte.to_bytes(length, byte_order, signed=signed))

class OutputFileLineStream():
    def __init__(self, filename):
        self.f = None
        self.filename = filename
        self.lines = 0

    def start(self):
        self.f = open(self.filename, 'w')

    def append(self, line):
        print(line, file=self.f)
        self.f.close()
        self.f = open(self.filename, 'a')
        self.lines += 1

    def close(self):
        self.f.close()

def get_episode_id(line):
    episode_id = line[1:].split(',')[0]
    if episode_id == 'episodeID':
        return None
    else:
        return int(episode_id)

class Reporter():
    def __init__(self):
        self.arduino = SerialLineStream()
        self.features = OutputFileLineStream(FEATURES_FILENAME)
        self.excluded_episodes = OutputFileLineStream(EXCLUSIONS_FILENAME)

        self.num_fields_expected = None
        self.current_episode = None
        self.continue_running = True

    def run(self):
        self.arduino.connect()
        self.features.start()
        self.excluded_episodes.start()
        try:
            while self.continue_running:
                line = self.arduino.get_line()
                if self.handle_handshake_line(line):
                    continue
                if self.handle_episode_line(line):
                    continue
                if self.handle_episode_error(line):
                    continue
                if self.handle_episode_warning(line):
                    continue
                if self.handle_initialization_message(line):
                    continue
                self.handle_malformed_line(line)
        except KeyboardInterrupt:
            pass
        print('Quitting...')
        self.features.close()
        self.excluded_episodes.close()

    def report_stats(self):
        print('Completed episode {}...'.format(self.current_episode))
        print('  {} lines written to output so far.'.format(self.features.lines))
        print('  {:.2f}% of all episodes have been excluded so far.'.format(
            self.excluded_episodes.lines * 100 / self.current_episode
        ))
        if NUM_EPISODES is not None:
            print('  Progress out of {} total episodes: {:.2f}%'.format(
                NUM_EPISODES, self.current_episode * 100 / NUM_EPISODES
            ))

    def handle_handshake_line(self, line):
        if line == '~':
            self.arduino.write_byte(0)
            return True
        return False

    def handle_episode_line(self, line):
        num_fields = line.count(',')
        if self.num_fields_expected is None:
            self.num_fields_expected = num_fields
            print('Number of fields in output:', self.num_fields_expected)
        if (line.startswith('[') and line.endswith(']') and
                num_fields == self.num_fields_expected):
            new_episode = get_episode_id(line)
            if (new_episode != self.current_episode) and (new_episode is not None):
                if (self.current_episode is not None) and (self.current_episode % 10 == 0):
                    self.report_stats()
                self.current_episode = new_episode
            if not (NUM_EPISODES is None or self.current_episode is None or
                    self.current_episode <= NUM_EPISODES):
                self.continue_running = False
                return True
            self.features.append(line[1:-1])
            return True
        return False

    def handle_episode_error(self, line):
        if self.current_episode is not None and line.startswith('E: '):
            print('Error in episode {}: {}'.format(self.current_episode, line[3:]))
            self.excluded_episodes.append(self.current_episode)
            return True
        return False

    def handle_episode_warning(self, line):
        if self.current_episode is not None and line.startswith('W: '):
            print('Warning in episode {}: {}'.format(self.current_episode, line[3:]))
            self.excluded_episodes.append(self.current_episode)
            return True
        return False

    def handle_initialization_message(self, line):
        if self.current_episode is None:
            print('Initialization message: {}'.format(line))
            return True
        return False

    def handle_malformed_line(self, line):
        print('Malformed line in episode {} or {}: {}'.format(
            self.current_episode, self.current_episode + 1, line
        ))
        self.excluded_episodes.append(self.current_episode)
        self.excluded_episodes.append(self.current_episode + 1)

def main():
    reporter = Reporter()
    reporter.run()


if __name__ == '__main__':
    main()

