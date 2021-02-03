import sys
import os
import FWCore.ParameterSet.Config as cms

sys.path.append(os.path.relpath("../../../../"))
from config_base import *

#--------------------

for c in all_cfg:
  timestamp_min = 58711
  timestamp_max = 76799

  c.timestamp_min = timestamp_min
  c.timestamp_max = timestamp_max
  c.timber_dir = "no_timber_dir"

  c.anal.alignment_t0 = timestamp_min
  c.anal.alignment_ts = (timestamp_max - timestamp_min) / 2.

  c.distilled_files = cms.vstring(
    "../../../fill7301/Totem1",
    # TODO: uncomment (when common alignment settings can be used)
    #"../../../fill7302/Totem1",
  )

  c.anal.use_3outof4_efficiency_fits = False
  c.anal.use_pileup_efficiency_fits = False

  # these alignment constants are only good for 7301
  c.anal.alignment_sources = cms.VPSet(
    cms.PSet(
      data = cms.VPSet(
        cms.PSet(unit=cms.string("L_2_F"), a = cms.double(0E-3), b = cms.double(+100E-3 - 40E-3), c = cms.double(+250E-3)),
        cms.PSet(unit=cms.string("L_1_F"), a = cms.double(0E-3), b = cms.double(+450E-3 + 40E-3), c = cms.double( -50E-3)),
        cms.PSet(unit=cms.string("R_1_F"), a = cms.double(0E-3), b = cms.double(+150E-3 - 40E-3), c = cms.double(-300E-3)),
        cms.PSet(unit=cms.string("R_2_F"), a = cms.double(0E-3), b = cms.double(+850E-3 + 40E-3), c = cms.double(-200E-3))
      )
    )
  )

#--------------------

cfg_45b_56t.anal.L_int = 1.

#--------------------

cfg_45t_56b.anal.L_int = 1.
