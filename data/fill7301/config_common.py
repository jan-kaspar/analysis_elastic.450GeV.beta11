import sys
import os
import FWCore.ParameterSet.Config as cms

tfd = os.path.dirname(os.path.abspath(__file__))
sys.path.append(tfd + "/../..")
from config_base import *

#--------------------

for c in all_cfg:
  timestamp_min = 58711
  timestamp_max = 62660

  c.timestamp_min = timestamp_min
  c.timestamp_max = timestamp_max
  c.timber_dir = "2018_10_14_fill7301"

  c.anal.alignment_t0 = timestamp_min
  c.anal.alignment_ts = (timestamp_max - timestamp_min) / 2.

#--------------------

# diagonal-independent alignment constants
a_L_2_F, b_L_2_F, c_L_2_F = 0E-3,  +84E-3, +199E-3
a_L_1_F, b_L_1_F, c_L_1_F = 0E-3, +442E-3,  -98E-3
a_R_1_F, b_R_1_F, c_R_1_F = 0E-3, +135E-3, -269E-3
a_R_2_F, b_R_2_F, c_R_2_F = 0E-3, +829E-3, -121E-3

#--------------------

for c in [cfg_45t_56t, cfg_45b_56t]:
  c.anal.alignment_sources = cms.VPSet(
    cms.PSet(
      data = cms.VPSet(
        cms.PSet(unit=cms.string("L_2_F"), a = cms.double(a_L_2_F + 0E-3), b = cms.double(b_L_2_F - 28E-3), c = cms.double(c_L_2_F + 5E-3)),
        cms.PSet(unit=cms.string("L_1_F"), a = cms.double(a_L_1_F + 0E-3), b = cms.double(b_L_1_F + 28E-3), c = cms.double(c_L_1_F - 5E-3)),
        cms.PSet(unit=cms.string("R_1_F"), a = cms.double(a_R_1_F + 0E-3), b = cms.double(b_R_1_F - 12E-3), c = cms.double(c_R_1_F + 30E-3)),
        cms.PSet(unit=cms.string("R_2_F"), a = cms.double(a_R_2_F + 0E-3), b = cms.double(b_R_2_F + 12E-3), c = cms.double(c_R_2_F - 30E-3))
      )
    )
  )

  c.anal.cut1_c = +2.9E-6
  c.anal.cut2_c = +0.6E-6

  c.anal.cut5_c = -0.015
  c.anal.cut6_c = +0.01

  c.anal.cut7_c = +0.00
  c.anal.cut8_c = +0.06

  c.anal.cut9_c = -0.00
  c.anal.cut10_c = -0.005

  c.anal.L_int = 1

#--------------------

for c in [cfg_45b_56b, cfg_45t_56b]:
  c.anal.alignment_sources = cms.VPSet(
    cms.PSet(
      data = cms.VPSet(
        cms.PSet(unit=cms.string("L_2_F"), a = cms.double(a_L_2_F + 0E-3), b = cms.double(b_L_2_F - 28E-3), c = cms.double(c_L_2_F - 26E-3)),
        cms.PSet(unit=cms.string("L_1_F"), a = cms.double(a_L_1_F + 0E-3), b = cms.double(b_L_1_F + 28E-3), c = cms.double(c_L_1_F + 26E-3)),
        cms.PSet(unit=cms.string("R_1_F"), a = cms.double(a_R_1_F + 0E-3), b = cms.double(b_R_1_F - 12E-3), c = cms.double(c_R_1_F - 4E-3)),
        cms.PSet(unit=cms.string("R_2_F"), a = cms.double(a_R_2_F + 0E-3), b = cms.double(b_R_2_F + 12E-3), c = cms.double(c_R_2_F + 4E-3))
      )
    )
  )

  c.anal.cut1_c = -3.6E-6
  c.anal.cut2_c = -0.7E-6

  c.anal.cut5_c = +0.02
  c.anal.cut6_c = -0.02

  c.anal.cut7_c = +0.00
  c.anal.cut8_c = -0.04

  c.anal.cut9_c = 0.01
  c.anal.cut10_c = +0.015

  c.anal.L_int = 1

#-------------------- automatically generated --------------------
cfg_45b_56t.anal.L_int = 121243.1
cfg_45t_56b.anal.L_int = 123367.5
