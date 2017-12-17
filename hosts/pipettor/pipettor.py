import random
import time
import argparse

import serialio

def map_range(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

class LineParser(object):
    def valid_line(self, line):
        return line.startswith('[') and line.endswith(']')

    def parse_line(self, line):
        return int(line[1:-1])

    def serialize_string(self, string):
        return '[{}]'.format(string)


class Pipettor(object):
    def __init__(self, arduino=None):
        if arduino is None:
            arduino = serialio.Arduino()
        self.arduino = arduino
        self.arduino.listeners.append(self)
        self.parser = LineParser()
        self.listeners = []
        self.running = False

        self.top_position = 11  # unitless
        self.top_mark = 0.95  # mL mark
        self.bottom_position = 999  # unitless
        self.bottom_mark = 0.03  # mL mark

    def setup(self):
        self.arduino.setup()
        print()

    def start(self):
        self.running = True
        try:
            while self.running:
                self.run()
        except KeyboardInterrupt:
            pass
        self.teardown()

    def run(self):
        line = self.arduino.get_line()
        self.on_read_line(line)

    def on_read_line(self, line):
        if self.handle_pipettor_line(line):
            return
        if self.handle_error(line):
            return
        if self.handle_warning(line):
            return
        self.handle_malformed_line(line)

    def teardown(self):
        print()
        print('Quitting...')
        self.arduino.reset()

    # Targeting

    def handle_pipettor_line(self, line):
        if self.parser.valid_line(line):
            parsed = self.parser.parse_line(line)
            print('Stabilized at the {:.2f} mL mark!'.format(self.to_mL_mark(parsed)))
            for listener in self.listeners:
                listener.on_stabilized_position(parsed, self.to_mL_mark(parsed))
            return True
        return False

    def set_target_position(self, target_position):
        self.arduino.write_line(self.parser.serialize_string(target_position))

    def set_target_mark(self, target_mark):
        self.set_target_position(self.to_unitless_position(target_mark))

    # Unit conversion

    def to_mL_mark(self, unitless_position):
        return map_range(unitless_position, self.bottom_position, self.top_position,
                         self.bottom_mark, self.top_mark)

    def to_unitless_position(self, mL_mark):
        return int(map_range(mL_mark, self.bottom_mark, self.top_mark,
                             self.bottom_position, self.top_position))

    # Exception handling

    def handle_error(self, line):
        if line.startswith('E: '):
            print('Error: {}'.format(line[3:]))
            return True
        return False

    def handle_warning(self, line):
        if line.startswith('W: '):
            print('Warning: {}'.format(line[3:]))
            return True
        return False

    def handle_malformed_line(self, line):
        print('Ignoring unexpected line: {}'.format(line))

class Targeting(object):
    def __init__(self, pipettor, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.pipettor = pipettor
        self.pipettor.listeners.append(self)

    def on_stabilized_position(self, position_unitless, position_mL_mark):
        pass

class RandomTargeting(Targeting):
    def on_stabilized_position(self, position_unitless, position_mL_mark):
        time.sleep(0.8)
        new_target = random.randint(self.pipettor.top_position, self.pipettor.bottom_position)
        self.pipettor.set_target_position(new_target)

class InteractiveTargeting(Targeting):
    def on_stabilized_position(self, position_unitless, position_mL_mark):
        need_input = True
        while need_input:
            try:
                user_input = input(
                    'Please specify the next position to go to between {} mL and {} mL: '
                    .format(self.pipettor.bottom_mark, self.pipettor.top_mark)
                )
                user_input = self.parse_input(user_input)
                need_input = False
            except ValueError:
                print('Invalid input: {}'.format(user_input))
                pass
            except EOFError:
                self.pipettor.running = False
                return
        self.pipettor.set_target_mark(user_input)

    def parse_input(self, user_input):
        if user_input.lower().endswith('ml'):
            user_input = user_input[:-2]
        user_input = user_input.strip()
        user_input = float(user_input)
        if (user_input < self.pipettor.bottom_mark or
                user_input > self.pipettor.top_mark):
            raise ValueError
        return user_input

def main(mode):
    pipettor = Pipettor()
    if mode == 'random':
        TargetingStrategy = RandomTargeting
    else:
        TargetingStrategy = InteractiveTargeting
    targeting = TargetingStrategy(pipettor)
    pipettor.setup()
    pipettor.start()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Control the pipettor module from the command line.'
    )
    parser.add_argument(
        'mode', choices=['random', 'interactive'], nargs='?', default='interactive',
        help=('Control mode. random generates target positions at random, while '
              'interactive waits for the user to enter a new target position every '
              'time the pipettor has reached its current target position.')
    )
    args = parser.parse_args()
    main(args.mode)

