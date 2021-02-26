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
  c.anal.alignment_ts = (timestamp_max - timestamp_min) / 2.

#--------------------

for c in [cfg_45t_56t, cfg_45b_56t]:
  c.anal.alignment_sources = cms.VPSet(
    cms.PSet(
      data = cms.VPSet(
        cms.PSet(unit=cms.string("L_2_F"), a = cms.double(0E-3), b = cms.double(+300E-3 - 25E-3), c = cms.double(+250E-3)),
        cms.PSet(unit=cms.string("L_1_F"), a = cms.double(0E-3), b = cms.double(+700E-3 + 25E-3), c = cms.double( -50E-3)),
        cms.PSet(unit=cms.string("R_1_F"), a = cms.double(0E-3), b = cms.double(-200E-3 + 20E-3), c = cms.double(-300E-3)),
        cms.PSet(unit=cms.string("R_2_F"), a = cms.double(0E-3), b = cms.double(+600E-3 - 20E-3), c = cms.double(-200E-3))
      )
    )
  )

  c.anal.cut1_c = +0.7E-6
  c.anal.cut2_c = +6.1E-6

  c.anal.cut5_c = +0.02
  c.anal.cut6_c = -0.009

  c.anal.cut7_c = -0.03
  c.anal.cut8_c = -0.09

  c.anal.cut9_c = +0.02
  c.anal.cut10_c = -0.02

  c.anal.L_int = 1

#--------------------

for c in [cfg_45b_56b, cfg_45t_56b]:
  c.anal.alignment_sources = cms.VPSet(
    cms.PSet(
      data = cms.VPSet(
        cms.PSet(unit=cms.string("L_2_F"), a = cms.double(0E-3), b = cms.double(+300E-3 - 20E-3), c = cms.double(+250E-3 - 0.2 * 0.465)),
        cms.PSet(unit=cms.string("L_1_F"), a = cms.double(0E-3), b = cms.double(+700E-3 + 20E-3), c = cms.double( -50E-3 - 0.2 * 0.422)),
        cms.PSet(unit=cms.string("R_1_F"), a = cms.double(0E-3), b = cms.double(-200E-3 + 20E-3), c = cms.double(-300E-3 + 0.3 * 0.399)),
        cms.PSet(unit=cms.string("R_2_F"), a = cms.double(0E-3), b = cms.double(+600E-3 - 20E-3), c = cms.double(-200E-3 + 0.3 * 0.438))
      )
    )
  )

  c.anal.cut1_c = +0.7E-6
  c.anal.cut2_c = -0.3E-6

  c.anal.cut5_c = -0.01
  c.anal.cut6_c = +0.00

  c.anal.cut7_c = -0.01
  c.anal.cut8_c = +0.11

  c.anal.cut9_c = +0.02
  c.anal.cut10_c = +0.02

  c.anal.L_int = 1
