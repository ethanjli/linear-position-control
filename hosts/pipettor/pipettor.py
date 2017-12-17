import random
import time

import serialio

class LineParser(object):
    def valid_line(self, line):
        return line.startswith('[') and line.endswith(']')

    def parse_line(self, line):
        return int(line[1:-1])

    def serialize_string(self, string):
        return '[{}]'.format(string)


class Pipettor(object):
    def __init__(self):
        self.arduino = serialio.Arduino()
        self.parser = LineParser()
        self.listeners = []
        self.running = True

        self.top_position = 11
        self.bottom_position = 999

    def setup(self):
        self.arduino.connect()
        self.arduino.wait_for_handshake()

    def run(self):
        try:
            while self.running:
                line = self.arduino.get_line()
                if self.handle_pipettor_line(line):
                    continue
                if self.handle_error(line):
                    continue
                if self.handle_warning(line):
                    continue
                self.handle_malformed_line(line)
        except KeyboardInterrupt:
            pass
        self.teardown()

    def teardown(self):
        print()
        print('Quitting...')
        self.arduino.reset()

    def handle_pipettor_line(self, line):
        if self.parser.valid_line(line):
            parsed = self.parser.parse_line(line)
            print('Stabilized at position {}!'.format(parsed))
            for listener in self.listeners:
                listener.on_stabilized_position(parsed)
            return True
        return False

    def set_target_position(self, target_position):
        self.arduino.write_line(self.parser.serialize_string(target_position))

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
    def __init__(self, pipettor):
        self.pipettor = pipettor

class RandomTargeting(Targeting):
    def on_stabilized_position(self, position):
        time.sleep(0.8)
        self.pipettor.set_target_position(random.randint(
            self.pipettor.top_position, self.pipettor.bottom_position
        ))

class UserTargeting(Targeting):
    def on_stabilized_position(self, position):
        need_input = True
        while need_input:
            try:
                user_input = int(input(
                    'Please specify the next position to go to (between {} and {}): '
                    .format(self.pipettor.top_position, self.pipettor.bottom_position)
                ))
                if (user_input < self.pipettor.top_position or
                        user_input > self.pipettor.bottom_position):
                    raise ValueError
                need_input = False
            except ValueError:
                print('Invalid input: {}'.format(user_input))
                pass
            except EOFError:
                self.pipettor.running = False
                return
        self.pipettor.set_target_position(user_input)

def main():
    pipettor = Pipettor()
    # targeting = RandomTargeting(pipettor)
    targeting = UserTargeting(pipettor)
    pipettor.listeners.append(targeting)
    pipettor.setup()
    pipettor.run()


if __name__ == '__main__':
    main()

