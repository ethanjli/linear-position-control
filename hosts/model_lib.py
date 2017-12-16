# Module containing PyTorch models.
from collections import OrderedDict
import math

import torch
from torch.autograd import Variable
from torch import nn
from torch.nn import functional as F
from torch.nn.utils.rnn import pad_packed_sequence
from torch.nn.utils.rnn import pack_padded_sequence


def xavier_init(linear):
  """Create and initialize weights and bias using Xavier initialization.
  Different variants of Xavier initialziation exist. For more info, see
  http://andyljones.tumblr.com/post/110998971763/an-explanation-of-xavier-initialization

  Args:
    linear: A torch.nn.Linear module.
  """
  assert isinstance(linear, nn.Linear)
  d_in = linear.in_features
  d_out = linear.out_features
  std = math.sqrt(2.0 / (d_in+d_out))
  linear.weight.data.normal_(std=std)
  linear.bias.data.zero_()


class LinearRegression(nn.Module):
  def __init__(self, dim_in):
    """
    Creates a linear regression model.

    Args:
      dim_in: Input dimension
    """
    super(LinearRegression, self).__init__()
    self.linear = nn.Linear(dim_in, 1)
    xavier_init(self.linear)

  def forward(self, x):
    y_pred = self.linear(x)
    return y_pred


def create_ff_model(dims, activation_fn=nn.ReLU):
  """Returns a feed-forward neural net with dimensions specified in `dims`.
  Non-linearity doesn't get applied to the last layer.

  Args:
    dims: list of dimensions where `dims[0]` is the input dimension and
          `dims[-1]` is the output dimension
    activation_fn: non-linearity to use
  """
  assert len(dims) > 1
  layers = OrderedDict()

  for i in range(len(dims)-1):
    dim_in, dim_out = dims[i], dims[i+1]
    layers['linear%d'%(i+1)] = nn.Linear(dim_in, dim_out)

    # Add non-linearity except for last layer
    if i < len(dims)-2:
      activation = activation_fn()
      name = activation_fn.__name__.lower()
      layers[name + str(i+1)] = activation

  model = nn.Sequential(layers)

  # Xavier initialization
  for m in model.modules():
    if isinstance(m, nn.Linear):
      xavier_init(m)

  return model


class SequentialRegression(nn.Module):
  def __init__(self, d_in, d_hidden, num_layers=1, regression_layers=None):
    super(SequentialRegression, self).__init__()
    self.d_in = d_in
    self.d_hidden = d_hidden
    self.num_layers = num_layers
    self.lstm = torch.nn.LSTM(d_in, d_hidden, num_layers, batch_first=True)
    self.linear = []
    d_prev = d_hidden
    if regression_layers:
      for i, ls in enumerate(regression_layers):
        linear = nn.Linear(d_prev, ls)
        xavier_init(linear)
        self.add_module('linear%d' % (i+1), linear)
        self.linear.append(linear)
        d_prev = ls
    self.final = nn.Linear(d_prev, 1)
    xavier_init(self.final)

  def forward(self, x, h, c, include_states=False):
    out, hc = self.lstm(x, (h, c))
    out, lengths = pad_packed_sequence(out, batch_first=True)
    result_size = out.size()[:2]
    out.contiguous()
    if self.linear:
      for linear in self.linear:
        out = linear(out.view(-1, linear.in_features))
        out = F.relu(out.view(-1, linear.out_features))
    pred = self.final(out)
    pred_packed = pack_padded_sequence(
        pred.view(*result_size), lengths, batch_first=True)

    # (batch, seq_len)
    if include_states:
      return pred_packed, hc[0], hc[1]
    else:
      return pred_packed

