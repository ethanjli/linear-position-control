import calibration

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

