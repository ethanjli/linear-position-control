import time
import collections

import serial

class SerialLineStream(object):
    def __init__(self):
        self.ser = None

    def connect(self, port='/dev/ttyACM0', baudrate=115200):
        print('Please connect the device now...')
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

    def close(self):
        self.ser.close()

class OutputFileLineStream(object):
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

class LineParser(object):
    def __init__(self):
        self.fields = None

    def line_as_csv(self, line):
        return line[1:-1]

    def parsed_as_csv(self, parsed):
        return ','.join(str(value) for value in parsed.values())

    def split(self, line):
        return tuple(self.line_as_csv(line).split(','))

    def count_fields(self, line):
        return line.count(',') + 1

    def valid_line(self, line):
        return (line.startswith('[') and line.endswith(']') and
                self.count_fields(line) == self.num_fields)

    def set_fields(self, header):
        self.fields = self.split(header)

    @property
    def num_fields(self):
        return len(self.fields)

    def parse_line(self, line):
        return collections.OrderedDict(zip(
            self.fields, (int(field) for field in self.split(line))
        ))

class Reporter(object):
    def __init__(self, features_filename='calibrationRig.csv',
                 exclusions_filename='exclusions.txt',
                 num_episodes=None, episode_report_interval=10):
        self.arduino = SerialLineStream()
        self.features = OutputFileLineStream(features_filename)
        self.excluded_episodes = OutputFileLineStream(exclusions_filename)
        self.parser = LineParser()

        self.num_fields_expected = None
        self.num_episodes = num_episodes
        self.current_episode = None
        self.episode_report_interval = episode_report_interval
        self.continue_running = True

    def setup(self):
        self.arduino.connect()
        self.features.start()
        self.excluded_episodes.start()

    def run(self):
        try:
            while self.continue_running:
                line = self.arduino.get_line()
                if self.handle_handshake_line(line):
                    continue
                if self.handle_header_line(line):
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
        self.arduino.close()

    def report_stats(self):
        print('Completed episode {}...'.format(self.current_episode))
        print('  {} lines written to output so far.'.format(self.features.lines))
        print('  {:.2f}% of all episodes have been excluded so far.'.format(
            self.excluded_episodes.lines * 100 / self.current_episode
        ))
        if self.num_episodes is not None:
            print('  Progress out of {} total episodes: {:.2f}%'.format(
                self.num_episodes, self.current_episode * 100 / self.num_episodes
            ))

    def handle_handshake_line(self, line):
        if line == '~':
            self.arduino.write_byte(0)
            return True
        return False

    def handle_header_line(self, line):
        if self.parser.fields is None:
            self.parser.set_fields(line)
            print('Number of fields in output:', self.parser.num_fields)
            self.features.append(self.parser.line_as_csv(line))
            return True
        return False

    def handle_episode_line(self, line):
        if self.parser.valid_line(line):
            parsed = self.parser.parse_line(line)
            parsed = self.process_episode_parsed(parsed)
            new_episode = parsed['episodeID']
            if (new_episode != self.current_episode) and (new_episode is not None):
                if (self.current_episode is not None and
                        self.current_episode % self.episode_report_interval == 0):
                    self.report_stats()
                self.current_episode = new_episode
            if self.current_episode is not None and self.current_episode > self.num_episodes:
                self.continue_running = False
                return True
            self.features.append(self.parser.parsed_as_csv(parsed))
            return True
        return False

    def process_episode_parsed(self, parsed):
        return parsed

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

class EpisodicController(Reporter):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

    def process_episode_parsed(self, parsed):
        return parsed

