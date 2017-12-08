import datetime
import bisect

import calibration

def find_closest(numbers, number):
    pos = bisect.bisect_left(numbers, number)
    if pos == 0:
        return numbers[0]
    if pos == len(numbers):
        return numbers[-1]
    before = numbers[pos - 1]
    after = numbers[pos]
    if after - number < number - before:
        return after
    else:
        return before

class Baseline(calibration.EpisodicController):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.action = None

    def choose_action(self, parsed):
        if parsed['estimatedPosition'] >= self.target_position and self.action > 0:
            self.action = 0
        elif parsed['estimatedPosition'] <= self.target_position and self.action < 0:
            self.action = 0
        return self.action

    def on_episode_start(self, parsed):
        if parsed['estimatedPosition'] < self.target_position:
            self.action = 150
        elif parsed['estimatedPosition'] > self.target_position:
            self.action = -150
        else:
            self.action = 0

class Oracle(calibration.EpisodicController):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        # Constants
        self.k_p = 6
        self.k_d = 20

        # Control terms
        self.error = None
        self.previous_error = None
        self.time = None
        self.previous_time = None
        self.d_t = None
        self.action = None

    def choose_action(self, parsed):
        self.error = self.target_position - parsed['groundTruthPosition']
        action = self.k_p * self.error
        if self.previous_error is not None:
            self.d_error = self.error - self.previous_error
            self.time = datetime.datetime.utcnow()
            self.d_t = (self.time - self.previous_time).microseconds / 1000
            action += self.k_d * self.d_error
        self.previous_error = self.error
        self.previous_time = self.time
        self.time = datetime.datetime.utcnow()
        action = self.nearest_action(action)
        self.action = action
        print(self.target_position, self.error, action)
        return self.action

    def on_episode_start(self, parsed):
        self.time = datetime.datetime.utcnow()
        self.previous_error = None
        self.previous_time = None

    def nearest_action(self, action):
        sign = 1 if action >= 0 else -1
        speed = abs(action)
        if speed < min(self.speeds) / 2:
            return 0
        speed = find_closest(self.speeds, speed)
        return sign * speed

