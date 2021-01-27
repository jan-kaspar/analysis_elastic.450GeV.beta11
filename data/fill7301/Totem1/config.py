import sys
import os
import FWCore.ParameterSet.Config as cms

from input_files import input_files

tfd = os.path.dirname(os.path.abspath(__file__))
sys.path.append(tfd + "/..")
from config_common import *

for c in all_cfg:
  c.input_files = input_files
