#!/usr/bin/env python3
import random

import controllers

# NUM_EPISODES = 20  # prototyping
NUM_EPISODES = 4000

# RANDOM_SEED = 0  # traindev
RANDOM_SEED = 1  # test


def main():
    random.seed(RANDOM_SEED)
    # Controller = controllers.Baseline
    # Controller = controllers.OraclePD
    # Controller = controllers.SlowOraclePD
    # Controller = controllers.LinearRegressionPD
    # Controller = controllers.SlowOracleLQG
    # Controller = controllers.LinearRegressionLQG
    Controller = controllers.LinearQ
    controller = Controller(num_episodes=NUM_EPISODES)
    controller.setup()
    controller.run()


if __name__ == '__main__':
    main()

