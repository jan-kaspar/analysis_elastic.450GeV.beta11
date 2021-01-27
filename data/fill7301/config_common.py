import sys
import os
import FWCore.ParameterSet.Config as cms

tfd = os.path.dirname(os.path.abspath(__file__))
sys.path.append(tfd + "/../..")
from config_base import *

#--------------------

for c in all_cfg:
  timestamp_min = TODO
  timestamp_max = TODO

  c.timestamp_min = timestamp_min
  c.timestamp_max = timestamp_max
  c.timber_dir = TODO # "2018_10_11_fill7280"

  c.anal.alignment_t0 = timestamp_min
  c.anal.alignment_ts = (timestamp_max - timestamp_min) / 2.

#--------------------

cfg_45b_56t.anal.L_int = TODO

#--------------------

cfg_45t_56b.anal.L_int = TODO
