from collections import namedtuple
import os, sys, time

from matplotlib import pyplot as plt
import numpy as np
import seaborn as sns
from tqdm import tqdm
import torch
from torch.nn.utils.rnn import pack_padded_sequence, pad_packed_sequence
import model_lib


TOP_FFN_MODELS = [
    'pytorch_models/ffn.hl_50_50.lr_0.1.wd_0.001.model.pt',   # Dev MSE: 69.3915
    'pytorch_models/ffn.hl_100_100.lr_0.1.wd_0.001.model.pt', # Dev MSE: 71.0149
    'pytorch_models/ffn.hl_50_50.lr_0.1.wd_0.01.model.pt',    # Dev MSE: 77.5178

]
TOP_RNN_MODELS = [
    'pytorch_models/lstm.hs_100.nl_2.lr_0.01.wd_0.001.rl_40.model.pt',    # Dev MSE: 89.5877
    'pytorch_models/lstm.hs_20.nl_1.lr_0.1.wd_0.001.rl_40_40.model.pt',  # Dev MSE: 90.2873
    'pytorch_models/lstm.hs_50.nl_1.lr_0.1.wd_0.001.rl_40_40.model.pt',   # Dev MSE: 91.4944
]


class Experiment(object):
  def __init__(self, name, stdout=True, dir=None):
    self.name = name
    self.filename = name + '.log'
    self.stdout = stdout
    if dir:
      self.filename = os.path.join(dir, self.filename)

  def __enter__(self):
    os.makedirs(os.path.dirname(self.filename), exist_ok=True)
    self.file = open(self.filename, 'w')
    self.log('Experiment %s logging started.' % self.name, stdout=False)
    return self

  def __exit__(self, exc_type, exc_value, traceback):
    self.log('Experiment %s logging ended.' % self.name, stdout=False)
    self.file.close()

  def get_time(self):
    return time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime())
    
  def log(self, msg, stdout=None, flush=True):
    lines = msg.split('\n')
    now = '[%s] ' % self.get_time()
    offset = len(now) * ' '

    self.file.write(now + lines[0] + '\n')
    for line in lines[1:]:
      self.file.write(offset + line + '\n')

    if (stdout is None and self.stdout 
        or stdout is not None and stdout):
      print('[%s] %s' % (self.name, msg))

    if flush:
      self.file.flush()

  def flush(self):
    self.file.flush()


class PositionEstimator:
  """Wrapper for loading & using trained PyTorch regression models."""

  def __init__(self, filename, stats_prefix='train_stats'):
    assert os.path.exists(filename)
    self.model = torch.load(filename)
    self.model.eval()
    print('Loaded model:\n' + str(self.model))
    if stats_prefix:
      self.means = np.load(stats_prefix + '_means.npy')
      self.stds = np.load(stats_prefix + '_stds.npy')
      print('Loaded stats for normalizing features')
    else:
      self.means = None
      self.stds = None

    # This is a RNN model, so we need to initialize hidden/cell states.
    if isinstance(self.model, model_lib.SequentialRegression):
      self.model_type = 'rnn'
      self.reset_rnn_states()
    else:
      self.model_type = 'ffn'

  def reset_rnn_states(self):
    self.h = torch.autograd.Variable(
        torch.zeros(
          self.model.lstm.num_layers, self.model.lstm.hidden_size),
        requires_grad=False)
    self.c = torch.autograd.Variable(
        torch.zeros(
          self.model.lstm.num_layers, self.model.lstm.hidden_size),
        requires_grad=False)

  def normalize_input(self, x):
    if self.means is not None and self.stds is not None:
      return (x - self.means) / self.stds

  def __call__(self, input):
    if isinstance(input, list):
      input = np.array(input)
    elif not isinstance(input, np.ndarray):
      raise ValueError('Input type %s not supported' % type(input))

    input = self.normalize_input(input)
    input = torch.from_numpy(input).float()
    input = torch.autograd.Variable(input, volatile=True)

    if self.model_type == 'ffn':
      scores = self.model(input)
    else:
      input = pack_padded_sequence(
          input.view(1, 1, -1), [input.size(0)], batch_first=True)
      scores, self.h, self.c = self.model(
          input, self.h, self.c, include_states=True)
      scores, _ = pad_packed_sequence(scores, batch_first=True)

    scores = scores.data.numpy()
    return scores.reshape(-1)


def smooth(x, n):
  cs = np.cumsum(np.insert(x, 0, 0))
  return (cs[n:] - cs[:-n]) / n


def plot_loss(fname, a, window=1, xlabel=None, ylabel=None):
  plt.clf()
  if window != 1:
    sa = smooth(a, window)
  else:
    sa = a
  plt.plot(range(1,len(sa)+1), sa)
  if xlabel:
    plt.xlabel(xlabel)
  if ylabel:
    plt.ylabel(ylabel)
  plt.savefig(fname)


def plot_losses(datas):
  plt.clf()
  sns.set_style("darkgrid")
  sns.set_palette("husl")
  for data in datas:
    plt.plot(smooth(data, 1000))


def plot_ql_losses(dir='results_dqn_basic'):
  datas = []
  datas.append(
      (np.load('./%s/dqn.hl_linear.lr_0.01.loss.npy' % dir),
      'Linear, lr=0.01'))
  datas.append(
      (np.load('./%s/dqn.hl_linear.lr_0.1.loss.npy' % dir),
      'Linear, lr=0.1'))
  datas.append(
      (np.load('./%s/dqn.hl_linear.lr_1.0.loss.npy' % dir),
      'Linear, lr=1.0'))
  datas.append(
      (np.load('./%s/dqn.hl_16.lr_0.01.loss.npy' % dir),
      'Hidden Size=16, lr=0.01'))
  datas.append(
      (np.load('./%s/dqn.hl_16.lr_0.1.loss.npy' % dir),
      'Hidden Size=16, lr=0.1'))
  datas.append(
      (np.load('./%s/dqn.hl_16.lr_1.0.loss.npy' % dir),
      'Hidden Size=16, lr=1.0'))
  datas.append(
      (np.load('./%s/dqn.hl_32.lr_0.1.loss.npy' % dir),
      'Hidden Size=32, lr=0.1'))
  datas.append(
      (np.load('./%s/dqn.hl_32.lr_0.01.loss.npy' % dir),
      'Hidden Size=32, lr=0.01'))
  datas.append(
      (np.load('./%s/dqn.hl_32.lr_1.0.loss.npy' % dir),
      'Hidden Size=32, lr=1.0'))

  def plot(datas, filename, yscale=None, legend_loc='right'):
    plt.rc('font', size=12)
    plt.clf()
    for data, name in datas:
      plt.plot(smooth(data, 1000), label=name)
    plt.legend(loc=legend_loc, fontsize=10)  # upper left corner
    plt.xlabel('Training Step')
    plt.ylabel('Loss')
    if yscale:
      plt.yscale(yscale)
      plt.ylabel('Log Loss')
    # plt.show()
    plt.savefig(filename)

  plot(datas[0:3], '%s/ql_losses.hl_linear.pdf' % dir)
  plot(datas[3:6], '%s/ql_losses.hl_16.pdf' % dir, yscale='log',
      legend_loc='lower right')
  plot(datas[6:9], '%s/ql_losses.hl_32.pdf' % dir, yscale='log',
      legend_loc='lower right')
