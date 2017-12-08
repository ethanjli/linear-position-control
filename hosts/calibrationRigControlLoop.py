#!/usr/bin/env python3
import calibration

NUM_EPISODES = 10  # prototyping
# NUM_EPISODES = 24000  # traindev
# NUM_EPISODES = 4000  # test

class HysteresisController(calibration.EpisodicController):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.action = None

    def choose_action(self, parsed):
        if parsed['groundTruthPosition'] >= self.target_position and self.action > 0:
            self.action = 0
        elif parsed['groundTruthPosition'] <= self.target_position and self.action < 0:
            self.action = 0
        return self.action

    def on_episode_start(self, parsed):
        if parsed['groundTruthPosition'] < self.target_position:
            self.action = 120
        elif parsed['groundTruthPosition'] > self.target_position:
            self.action = -120
        else:
            self.action = 0

def main():
    controller = HysteresisController(num_episodes=NUM_EPISODES, episode_report_interval=1)
    controller.setup()
    controller.run()


if __name__ == '__main__':
    main()

