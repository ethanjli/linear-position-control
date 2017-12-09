import datetime
import bisect

import numpy as np

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

class PD(calibration.EpisodicController):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        # Constants
        self.k_p = None
        self.k_d = None

        # Control terms
        self.error = None
        self.previous_error = None
        self.time = None
        self.previous_time = None
        self.d_t = None
        self.action = None

    def estimate_position(self, parsed):
        pass

    def choose_action(self, parsed):
        self.error = self.target_position - self.estimate_position(parsed)
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

class Oracle(PD):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.k_p = 6
        self.k_d = 20

    def estimate_position(self, parsed):
        return parsed['groundTruthPosition']

class LinearRegression(PD):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.k_p = 2
        self.k_d = 2
        self.terms = [
            'lastLimit', 'backwardsEdgesSinceLastLimit', 'forwardsEdgesSinceLastLimit',
            'atRightLimit', 'motorDirection', 'atLeftLimit', 'opticalSensor'
        ]
        self.coefs = [451.4526, -22.8686, 22.6784, -12.3514, -3.6129, -3.2660, -0.3014]
        self.intercept = 515.8647
        self.speeds = [120, 130, 140, 150, 160, 170, 180, 190, 200]

    def estimate_position(self, parsed):
        terms = [parsed[term] for term in self.terms]
        return self.intercept + np.dot(self.coefs, terms)

