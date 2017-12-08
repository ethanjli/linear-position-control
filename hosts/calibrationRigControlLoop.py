#!/usr/bin/env python3
import random

import calibration

NUM_EPISODES = 10  # prototyping
# NUM_EPISODES = 24000  # traindev
# NUM_EPISODES = 4000  # test

# RANDOM_SEED = 0  # traindev
RANDOM_SEED = 1  # test

class HysteresisController(calibration.EpisodicController):
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

def main():
    random.seed(RANDOM_SEED)
    controller = HysteresisController(num_episodes=NUM_EPISODES, episode_report_interval=1)
    controller.setup()
    controller.run()


if __name__ == '__main__':
    main()

