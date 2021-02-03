import sys
import os
import FWCore.ParameterSet.Config as cms

sys.path.append(os.path.relpath("../../"))
from config_common import *

cfg_45b_56t.anal.fc_G = FiducialCut([[-155E-6, 400E-6], [-150E-6, 160E-6], [+150E-6, 160E-6], [+155E-6, 400E-6], [+150E-6, 500E-6], [-150E-6, 500E-6]])

cfg_45t_56b.anal.fc_G = FiducialCut([[-155E-6, 400E-6], [-150E-6, 180E-6], [+150E-6, 180E-6], [+155E-6, 400E-6], [+150E-6, 500E-6], [-150E-6, 500E-6]])
