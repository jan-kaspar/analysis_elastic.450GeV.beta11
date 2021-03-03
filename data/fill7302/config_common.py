import sys
import os
import FWCore.ParameterSet.Config as cms

tfd = os.path.dirname(os.path.abspath(__file__))
sys.path.append(tfd + "/../..")
from config_base import *

#--------------------

for c in all_cfg:
  timestamp_min = 69013
  timestamp_max = 76799

  c.timestamp_min = timestamp_min
  c.timestamp_max = timestamp_max
  c.timber_dir = "2018_10_14_fill7302"

  c.anal.alignment_t0 = timestamp_min
  c.anal.alignment_ts = (timestamp_max - timestamp_min) / 4.

#--------------------

# diagonal-independent alignment constants
a_L_2_F, b_L_2_F, c_L_2_F = 0E-3, +290E-3, +199E-3
a_L_1_F, b_L_1_F, c_L_1_F = 0E-3, +680E-3,  -98E-3
a_R_1_F, b_R_1_F, c_R_1_F = 0E-3, -167E-3, -269E-3
a_R_2_F, b_R_2_F, c_R_2_F = 0E-3, +579E-3, -121E-3

#--------------------

for c in [cfg_45t_56t, cfg_45b_56t]:
  c.anal.alignment_sources = cms.VPSet(
    cms.PSet(
      data = cms.VPSet(
        cms.PSet(unit=cms.string("L_2_F"), a = cms.double(a_L_2_F + 0E-3), b = cms.double(b_L_2_F - 0E-3), c = cms.double(c_L_2_F + 0E-3)),
        cms.PSet(unit=cms.string("L_1_F"), a = cms.double(a_L_1_F + 0E-3), b = cms.double(b_L_1_F + 0E-3), c = cms.double(c_L_1_F + 0E-3)),
        cms.PSet(unit=cms.string("R_1_F"), a = cms.double(a_R_1_F + 0E-3), b = cms.double(b_R_1_F - 0E-3), c = cms.double(c_R_1_F + 0E-3)),
        cms.PSet(unit=cms.string("R_2_F"), a = cms.double(a_R_2_F + 0E-3), b = cms.double(b_R_2_F + 0E-3), c = cms.double(c_R_2_F + 0E-3))
      )
    )
  )

  c.anal.cut1_c = +21.3E-6
  c.anal.cut2_c = +26.2E-6

  c.anal.cut5_c = +0.07
  c.anal.cut6_c = -0.009

  c.anal.cut7_c = +0.04
  c.anal.cut8_c = -0.23

  c.anal.cut9_c = +0.005
  c.anal.cut10_c = +0.05

  c.anal.L_int = 1

#--------------------

for c in [cfg_45b_56b, cfg_45t_56b]:
  c.anal.alignment_sources = cms.VPSet(
    cms.PSet(
      data = cms.VPSet(
        cms.PSet(unit=cms.string("L_2_F"), a = cms.double(a_L_2_F + 0E-3), b = cms.double(b_L_2_F - 0E-3), c = cms.double(c_L_2_F - 0. * 0.465)),
        cms.PSet(unit=cms.string("L_1_F"), a = cms.double(a_L_1_F + 0E-3), b = cms.double(b_L_1_F + 0E-3), c = cms.double(c_L_1_F - 0. * 0.422)),
        cms.PSet(unit=cms.string("R_1_F"), a = cms.double(a_R_1_F + 0E-3), b = cms.double(b_R_1_F - 0E-3), c = cms.double(c_R_1_F + 0. * 0.399)),
        cms.PSet(unit=cms.string("R_2_F"), a = cms.double(a_R_2_F + 0E-3), b = cms.double(b_R_2_F + 0E-3), c = cms.double(c_R_2_F + 0. * 0.438))
      )
    )
  )

  c.anal.cut1_c = +16.7E-6
  c.anal.cut2_c = +20.0E-6

  c.anal.cut5_c = +0.03
  c.anal.cut6_c = +0.006

  c.anal.cut7_c = +0.05
  c.anal.cut8_c = +0.03

  c.anal.cut9_c = +0.005
  c.anal.cut10_c = +0.07

  c.anal.L_int = 1
