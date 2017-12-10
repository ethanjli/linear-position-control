import datetime
import bisect

import numpy as np
import controlpy as cp

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
        self.action = self.nearest_action(action)
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

class LQG(calibration.EpisodicController):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.velocity_gain = 15
        self.velocity_smoothness = 0.8
        self.motor_gain = 8
        self.motor_normalization = 1 / 200

        # Dynamics
        self.A = np.array([
            [1, self.velocity_gain],  # distance at full velocity
            [0, self.velocity_smoothness]
        ])
        self.B = np.array([
            [0],
            [1 - self.velocity_smoothness]
        ])

        # LQR
        self.Q_LQR = np.array([
            [1, 0],
            [0, 0]
        ])
        self.R_LQR = np.eye(1)
        self.K_LQR = None
        self.compute_LQR_gain()

        # Kalman
        self.H_kalman = np.array([[1, 0]])
        self.Q_kalman = np.eye(2)
        self.R_kalman = np.eye(1)
        self.K_kalman = None
        self.compute_kalman_gain()

        self.state = None
        self.action = None

    def compute_LQR_gain(self):
        (self.K_LQR, _, __) = cp.synthesis.controller_lqr_discrete_time(
            self.A, self.B, self.Q_LQR, self.R_LQR)

    def compute_kalman_gain(self):
        (self.K_kalman, _, __) = cp.synthesis.estimator_kalman_steady_state_discrete_time(
            self.A, self.H_kalman, self.Q_kalman, self.R_kalman)
        self.state_kalman = np.dot(np.eye(2) - np.dot(self.K_kalman, self.H_kalman), self.A)
        self.action_kalman = np.dot(np.eye(1) - np.dot(self.K_kalman, self.H_kalman), self.B)

    def estimate_position(self, parsed):
        pass

    def estimate_state(self, position, state, action):
        return (np.dot(self.state_kalman, state) +
                np.dot(self.action_kalman, action * self.motor_normalization) +
                np.dot(self.K_kalman, position))

    def choose_action(self, parsed):
        self.state = self.estimate_state(self.estimate_position(parsed),
                                         self.state, self.action)
        control_state = self.state - np.array([[self.target_position], [0]])
        action = (-np.dot(self.K_LQR, control_state))[0][0]
        self.action = self.nearest_action(action * self.motor_gain)
        # print(parsed['groundTruthPosition'], self.target_position, self.state[0][0], control_state[0][0], action)
        return self.action

    def on_episode_start(self, parsed):
        self.state = np.array([[self.estimate_position(parsed)], [0]])
        self.action = 0

    def nearest_action(self, action):
        sign = 1 if action >= 0 else -1
        speed = abs(action)
        if speed < min(self.speeds) / 2:
            return 0
        speed = find_closest(self.speeds, speed)
        return sign * speed

class OraclePD(PD):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.k_p = 6
        self.k_d = 20

    def estimate_position(self, parsed):
        return parsed['groundTruthPosition']

class SlowOraclePD(OraclePD):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.k_p = 2
        self.k_d = 2
        self.speeds = [120, 130, 140, 150, 160, 170, 180, 190, 200]

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

class SlowOracleLQG(LQG):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.speeds = [120, 130, 140, 150, 160, 170, 180, 190, 200]

    def estimate_position(self, parsed):
        return parsed['groundTruthPosition']

