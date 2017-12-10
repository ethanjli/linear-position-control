from collections import namedtuple
import os, sys, time

from matplotlib import pyplot as plt
import numpy as np
from tqdm import tqdm
import torch


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
    
  def log(self, msg, stdout=None):
    lines = msg.split('\n')
    now = '[%s] ' % self.get_time()
    offset = len(now) * ' '

    self.file.write(now + lines[0] + '\n')
    for line in lines[1:]:
      self.file.write(offset + line + '\n')

    if (stdout is None and self.stdout 
        or stdout is not None and stdout):
      print(msg)

  def flush(self):
    self.file.flush()


class TrainedRLPolicy:
  """Wrapper for loading & using trained PyTorch RL models from file."""

  def __init__(self, filename):
    assert os.path.exists(filename)
    self.model = torch.load(filename)
    self.model.eval()
    print('Loaded model:\n' + str(self.model))

  def __call__(self, input):
    if isinstance(input, list):
      input = torch.FloatTensor(input)
    elif isinstance(input, np.ndarray):
      input = torch.from_numpy(input).float()
    else:
      raise ValueError('Input type %s not supported' % type(input))

    input = torch.autograd.Variable(input, volatile=True)
    scores = self.model(input)
    scores = scores.data.numpy()

    if scores.ndim == 1:
      return scores.argmax()
    else:
      return scores.argmax(axis=1)


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

