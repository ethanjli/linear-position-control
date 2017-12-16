#!/usr/bin/env python3
import calibration

# NUM_EPISODES = 24000  # traindev
NUM_EPISODES = 4000  # test

def main():
    reporter = calibration.Reporter(num_episodes=NUM_EPISODES)
    reporter.setup()
    reporter.run()


if __name__ == '__main__':
    main()

