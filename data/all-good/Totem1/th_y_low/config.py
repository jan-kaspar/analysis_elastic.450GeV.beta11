import sys
import os
import FWCore.ParameterSet.Config as cms

sys.path.append(os.path.relpath("../../"))
from config_common import *

cfg_45b_56t.anal.fc_G = FiducialCut([[-400E-6, 160E-6], [+200E-6, 160E-6], [+540E-6, 220E-6], [+500E-6, 300E-6], [-500E-6, 300E-6], [-540E-6, 220E-6]])

cfg_45t_56b.anal.fc_G = FiducialCut([[-200E-6, 180E-6], [+400E-6, 180E-6], [+520E-6, 220E-6], [+480E-6, 300E-6], [-480E-6, 300E-6], [-520E-6, 220E-6]])
