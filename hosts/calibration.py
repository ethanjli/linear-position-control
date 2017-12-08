import collections
import random
import datetime

import iostreams

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
        self.arduino = iostreams.SerialLineStream()
        self.features = iostreams.OutputFileLineStream(features_filename)
        self.excluded_episodes = iostreams.OutputFileLineStream(exclusions_filename)
        self.parser = LineParser()

        self.num_fields_expected = None
        self.num_episodes = num_episodes
        self.report_episode = None
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
        self.arduino.reset()

    def report_stats(self):
        print('Completed episode {}...'.format(self.report_episode))
        print('  {} lines written to output so far.'.format(self.features.lines))
        print('  {:.2f}% of all episodes have been excluded so far.'.format(
            self.excluded_episodes.lines * 100 / self.report_episode
        ))
        if self.num_episodes is not None:
            print('  Progress out of {} total episodes: {:.2f}%'.format(
                self.num_episodes, self.report_episode * 100 / self.num_episodes
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
            parsed = self.process_parsed(parsed)
            if parsed is None:
                return True
            report_episode = parsed['episodeID']
            if (report_episode != self.report_episode) and (report_episode is not None):
                if (self.report_episode is not None and
                        self.report_episode % self.episode_report_interval == 0):
                    self.report_stats()
                self.report_episode = report_episode
            if self.report_episode is not None and self.report_episode > self.num_episodes:
                self.continue_running = False
                return True
            self.features.append(self.parser.parsed_as_csv(parsed))
            return True
        return False

    def process_parsed(self, parsed):
        return parsed

    # Exception handling

    def handle_episode_error(self, line):
        if self.report_episode is not None and line.startswith('E: '):
            print('Error in episode {}: {}'.format(self.report_episode, line[3:]))
            self.excluded_episodes.append(self.report_episode)
            return True
        return False

    def handle_episode_warning(self, line):
        if self.report_episode is not None and line.startswith('W: '):
            print('Warning in episode {}: {}'.format(self.report_episode, line[3:]))
            self.excluded_episodes.append(self.report_episode)
            return True
        return False

    def handle_initialization_message(self, line):
        if self.report_episode is None:
            print('Initialization message: {}'.format(line))
            return True
        return False

    def handle_malformed_line(self, line):
        print('Malformed line in episode {} or {}: {}'.format(
            self.report_episode, self.report_episode + 1, line
        ))
        self.excluded_episodes.append(self.report_episode)
        self.excluded_episodes.append(self.report_episode + 1)

class EpisodicController(Reporter):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.speeds = [120, 130, 140, 150, 160, 170, 180, 190, 200, 210, 220, 230, 240]
        self.actions = [
            0, 120, -120, 130, -130, 140, -140, 150, -150, 160, -160, 170, -170,
            180, -180, 190, -190, 200, -200, 210, -210, 220, -220, 230, -230, 240, -240
        ]
        self.action_codes = {action: index for (index, action) in enumerate(self.actions)}
        self.position_range = (64, 955)
        self.episode_time_limit = 1  # seconds

        # Current episode
        self.current_episode = -1
        self.target_position = None
        self.episode_start = None

        # Pre-episode localization
        self.pre_episode_localization_direction = None
        self.pre_episode_localization_speed = 150

    def process_parsed(self, parsed):
        if self.target_position is None:  # episode just completed
            if self.current_episode == -1:  # device startup localization to clear all timers
                self.start_pre_episode_localization(direction=-1)
            else:  # normal pre-episode localization
                self.start_pre_episode_localization()
            return None
        elif self.target_position == -3:  # pre-episode localization completed
            self.start_new_episode(parsed)
            self.write_action(0)
            return None
        elif self.target_position < 0:  # pre-episode localization
            if self.pre_episode_localization_complete(parsed):
                self.target_position -= 1
            self.write_action(self.pre_episode_localization_direction *
                              self.pre_episode_localization_speed)
            return None
        self.write_action(self.choose_action(parsed))
        # TODO: compute reward
        if self.episode_complete(parsed):
            self.target_position = None
        parsed['episodeID'] = self.current_episode
        parsed['targetingTimeMicroseconds'] = self.episode_time.microseconds
        if self.current_episode > 0:
            return parsed
        else:
            return None

    # Episode management

    def start_new_episode(self, parsed):
        self.current_episode += 1
        self.target_position = random.randint(*self.position_range)
        self.episode_start = datetime.datetime.utcnow()
        self.on_episode_start(parsed)

    def start_pre_episode_localization(self, direction=None):
        if direction is None:
            direction = random.choice((-1, 1))
        self.target_position = -1
        self.pre_episode_localization_direction = direction
        self.write_action(direction * self.pre_episode_localization_speed)

    def pre_episode_localization_complete(self, parsed):
        return ((self.pre_episode_localization_direction == -1 and parsed['atLeftLimit']) or
                (self.pre_episode_localization_direction == 1 and parsed['atRightLimit']))

    def episode_complete(self, parsed):
        if self.episode_time.seconds > self.episode_time_limit:
            return True
        return False

    def on_episode_start(self, parsed):
        pass

    # Action management

    def choose_action(self, parsed):
        return 0

    def encode_action(self, action):
        return self.action_codes[action]

    def write_action(self, action):
        self.arduino.write_byte(self.encode_action(action))

    # Utilities

    @property
    def episode_time(self):
        return datetime.datetime.utcnow() - self.episode_start

