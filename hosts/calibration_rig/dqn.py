from collections import OrderedDict
import math
import random

import ipdb
import torch
from torch.autograd import Variable
from torch.nn import functional as F
from torch import nn


# Parameters for e-decay
EPS_START = 0.9
EPS_END = 0.05
EPS_DECAY = 200


class DQN(nn.Module):
  """The Q-Network for Deep Q-Learning implementation."""

  def __init__(self, d_state, n_actions, layers=None):
    """Sets up the Q-Network of specified layer sizes.

    Args:
      d_state: Dimension of the input (state) vector.
      n_actions: Number of (discrete) actions.
      layers: List of hidden layer sizes.
    """
    super(DQN, self).__init__()
    self.d_state = d_state
    self.n_actions = n_actions
    self.hidden_layers = []
    prev_size = d_state

    if layers:
      for i, size in enumerate(layers):
        linear = nn.Linear(prev_size, size)
        self.hidden_layers.append(linear)
        self.add_module(('hidden%d' % i), linear)
        prev_size = size

    self.final = nn.Linear(prev_size, n_actions)


  def forward(self, x):
    """Assume x.size() == (batch_size, d_state)"""
    if self.hidden_layers:
      for hidden in self.hidden_layers:
        x = F.relu(hidden(x))
    x = self.final(x)

    return x


def get_reward(positions, targets):
  scores = 10 - (positions - targets)**2 / 10.
  max_vals = torch.max(Variable(torch.zeros(1,1)), scores)
  return torch.squeeze(max_vals)


# def eps_greedy(state, n_actions, step, model):
#   eps = EPS_END + (EPS_START - EPS_END) * np.exp(-1 * step / EPS_DECAY)
#   sample = random.random()
#   if sample > eps:
#     # TODO: greedy action
#     return 1
#   else:
#     # Random action
#     return 1


