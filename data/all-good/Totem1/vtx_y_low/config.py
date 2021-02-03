import sys
import os
import FWCore.ParameterSet.Config as cms

sys.path.append(os.path.relpath("../../"))
from config_common import *

for c in all_cfg:
    c.anal.vtx_y_max = -0.1
