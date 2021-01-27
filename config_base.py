import FWCore.ParameterSet.Config as cms
import math

#----------------------------------------------------------------------------------------------------

def FiducialCut(points):
  c = cms.VPSet()
  for p in points:
    c.append(cms.PSet(x=cms.double(p[0]), y=cms.double(p[1])))
  return c

#----------------------------------------------------------------------------------------------------

cfg = cms.PSet(
  input_files = cms.vstring(),
  distilled_files = cms.vstring("."),

  timestamp0 = cms.double(1539208800),

  timestamp_min = cms.double(0.),
  timestamp_max = cms.double(0.),

  timber_dir = cms.string(""),

  env = cms.PSet(
    use_matched_optics = cms.bool(False)
  ),

  anal = cms.PSet(
    excl_timeIntervals = cms.VPSet(
      # fill 7280, high PU prob
      cms.PSet(first=cms.uint32(78680), second=cms.uint32(79259)),
      cms.PSet(first=cms.uint32(79920), second=cms.uint32(79979)),
      cms.PSet(first=cms.uint32(80640), second=cms.uint32(81059)),
      cms.PSet(first=cms.uint32(84840), second=cms.uint32(84964)),

      # fill 7281, high PU prob
      cms.PSet(first=cms.uint32(93360), second=cms.uint32(93479)),
      cms.PSet(first=cms.uint32(94440), second=cms.uint32(94499)),
      cms.PSet(first=cms.uint32(101500), second=cms.uint32(101600)), # tiny end of fill

      # fill 7282
      cms.PSet(first=cms.uint32(105700), second=cms.uint32(105899)), # high PU prob
      cms.PSet(first=cms.uint32(107520), second=cms.uint32(107556)), # high PU prob
      cms.PSet(first=cms.uint32(112100), second=cms.uint32(112400)), # th_y resolution degraded
      cms.PSet(first=cms.uint32(112380), second=cms.uint32(112559)), # high PU prob
      cms.PSet(first=cms.uint32(115000), second=cms.uint32(117000)), # th_y resolution degraded

      # fill 7283, additional 3/4 inefficiency (PU prob >~ 1%)
      cms.PSet(first=cms.uint32(121000), second=cms.uint32(123000)),
      cms.PSet(first=cms.uint32(124400), second=cms.uint32(127000)),

      # fill 7284, high PU prob
      cms.PSet(first=cms.uint32(131220), second=cms.uint32(131339)),
      cms.PSet(first=cms.uint32(133920), second=cms.uint32(134339)),
      cms.PSet(first=cms.uint32(137640), second=cms.uint32(137759)),
      cms.PSet(first=cms.uint32(141420), second=cms.uint32(141659)),

      # fill 7285, high PU prob
      cms.PSet(first=cms.uint32(150480), second=cms.uint32(150634)),
      cms.PSet(first=cms.uint32(152340), second=cms.uint32(152969)),

      # additional 3/4 inefficiency (PU prob >~ 1%)
      cms.PSet(first=cms.uint32(148000), second=cms.uint32(150600)),
      cms.PSet(first=cms.uint32(151600), second=cms.uint32(152600)),
      cms.PSet(first=cms.uint32(154300), second=cms.uint32(160000)),

      # fill 7289, high PU prob
      cms.PSet(first=cms.uint32(206000), second=cms.uint32(206099)),

      # fill 7291, high PU prob
      cms.PSet(first=cms.uint32(233078), second=cms.uint32(233099)),
    ),

    excl_bunches = cms.vuint32(),

    excl_runs = cms.vuint32(324467),

    excl_lsIntervals = cms.VPSet(
      # horizontal RPs inserted - decreased efficiency

      # fill 7281
      cms.PSet(run=cms.uint32(324462), ls_first=cms.uint32( 94), ls_second=cms.uint32(338)),

      # fill 7289
      cms.PSet(run=cms.uint32(324532), ls_first=cms.uint32(399), ls_second=cms.uint32(520)),

      # fill 7291
      cms.PSet(run=cms.uint32(324536), ls_first=cms.uint32(360), ls_second=cms.uint32(509))
    ),

    alignment_sources = cms.VPSet(),

    t_min = cms.double(0.),
    t_max = cms.double(0.03),
    t_min_full = cms.double(0.),
    t_max_full = cms.double(0.045),
    t_min_fit = cms.double(0.),

    n_si = cms.double(4.0),

    cut1_a = cms.double(0.), cut1_c = cms.double(0.), cut1_si = cms.double(0.),
    cut2_a = cms.double(0.), cut2_c = cms.double(0.), cut2_si = cms.double(0.),
    cut3_a = cms.double(0.), cut3_c = cms.double(0.), cut3_si = cms.double(0.),
    cut4_a = cms.double(0.), cut4_c = cms.double(0.), cut4_si = cms.double(0.),
    cut5_a = cms.double(0.), cut5_c = cms.double(0.), cut5_si = cms.double(0.),
    cut6_a = cms.double(0.), cut6_c = cms.double(0.), cut6_si = cms.double(0.),
    cut7_a = cms.double(0.), cut7_c = cms.double(0.), cut7_si = cms.double(0.),
    cut8_a = cms.double(0.), cut8_c = cms.double(0.), cut8_si = cms.double(0.),
    cut9_a = cms.double(0.), cut9_c = cms.double(0.), cut9_si = cms.double(0.),
    cut10_a = cms.double(0.), cut10_c = cms.double(0.), cut10_si = cms.double(0.),

    fc_L = FiducialCut([]),
    fc_R = FiducialCut([]),
    fc_G = FiducialCut([]),

    vtx_x_min = cms.double(-1E100),
    vtx_x_max = cms.double(+1E100),
    vtx_y_min = cms.double(-1E100),
    vtx_y_max = cms.double(+1E100),

    si_th_x_1arm_L = cms.double(0.),
    si_th_x_1arm_R = cms.double(0.),
    si_th_x_1arm_unc = cms.double(0.),
    si_th_x_2arm = cms.double(0.),
    si_th_x_2arm_unc = cms.double(0.),
    si_th_x_LRdiff = cms.double(0.),
    si_th_x_LRdiff_unc = cms.double(0.),

    si_th_y_1arm = cms.double(0.),
    si_th_y_1arm_unc = cms.double(0.),
    si_th_y_2arm = cms.double(0.),
    si_th_y_2arm_unc = cms.double(0.),
    si_th_y_LRdiff = cms.double(0.),
    si_th_y_LRdiff_unc = cms.double(0.),

    use_resolution_fits = cms.bool(True),

    use_3outof4_efficiency_fits = cms.bool(True),
    use_pileup_efficiency_fits = cms.bool(True),

    inefficiency_3outof4 = cms.double(0.),
    inefficiency_shower_near = cms.double(0.03),
    inefficiency_pile_up = cms.double(0.),
    inefficiency_trigger = cms.double(0.),
    inefficiency_DAQ = cms.double(0.),

    bckg_corr = cms.double(1.),

    L_int = cms.double(0.),

    eff_3outof4_fixed_point = cms.double(0.),
    eff_3outof4_slope = cms.double(0.),
    eff_3outof4_slope_unc = cms.double(0.),

    norm_corr = cms.double(0.),
    norm_corr_unc = cms.double(0.),

    alignment_t0 = cms.double(0.),
    alignment_ts = cms.double(60*60),

    binnings = cms.vstring("ub", "eb"),

    unsmearing_file = cms.string("unfolding_cf_ni_<diagonal>.root"),
    unsmearing_object = cms.string("fit-1/<binning>"),

    alignment_y_ranges = cms.VPSet(
      cms.PSet(unit=cms.string("L_2_F"), bot_min=cms.double(-24.0), bot_max=cms.double(-6.9), top_min=cms.double(7.4), top_max=cms.double(+25.5)),
      cms.PSet(unit=cms.string("L_1_F"), bot_min=cms.double(-21.5), bot_max=cms.double(-6.3), top_min=cms.double(6.6), top_max=cms.double(+22.3)),
      cms.PSet(unit=cms.string("R_1_F"), bot_min=cms.double(-23.0), bot_max=cms.double(-6.6), top_min=cms.double(6.6), top_max=cms.double(+21.5)),
      cms.PSet(unit=cms.string("R_2_F"), bot_min=cms.double(-25.2), bot_max=cms.double(-7.0), top_min=cms.double(7.3), top_max=cms.double(+24.0))
    )
  )
)

si_th_y_1arm = 6E-6
cfg.anal.si_th_y_1arm = si_th_y_1arm
cfg.anal.si_th_y_1arm_unc = -999.

cfg.anal.si_th_y_LRdiff = si_th_y_1arm * math.sqrt(2.)
cfg.anal.si_th_y_LRdiff_unc = -999.

cfg.anal.si_th_y_2arm = si_th_y_1arm / math.sqrt(2.)
cfg.anal.si_th_y_2arm_unc = -999.

si_th_x_1arm = 23E-6
cfg.anal.si_th_x_1arm_L = si_th_x_1arm
cfg.anal.si_th_x_1arm_R = si_th_x_1arm
cfg.anal.si_th_x_1arm_unc = -999.

cfg.anal.si_th_x_LRdiff = si_th_x_1arm * math.sqrt(2.)
cfg.anal.si_th_x_LRdiff_unc = -999.

cfg.anal.si_th_x_2arm = si_th_x_1arm / math.sqrt(2.)
cfg.anal.si_th_x_2arm_unc = -999.

#----------------------------------------------------------------------------------------------------

# diagonal-independent alignment constants
a_L_2_F, b_L_2_F, c_L_2_F = -6.0E-3,  -16E-3, +158E-3
a_L_1_F, b_L_1_F, c_L_1_F = -6.0E-3, +421E-3,   +8E-3
a_R_1_F, b_R_1_F, c_R_1_F = +6.0E-3, +101E-3, -236E-3
a_R_2_F, b_R_2_F, c_R_2_F = +6.0E-3, +863E-3,   -0E-3

#----------------------------------------------------------------------------------------------------

cfg_45b_56t = cfg.clone(
  anal = dict(
    alignment_sources = cms.VPSet(
      cms.PSet(
        data = cms.VPSet(
          cms.PSet(unit=cms.string("L_2_F"), a = cms.double(a_L_2_F + 2E-3 - 0.3E-3), b = cms.double(b_L_2_F - 17E-3), c = cms.double(c_L_2_F - 11E-3 - 90E-3)),
          cms.PSet(unit=cms.string("L_1_F"), a = cms.double(a_L_1_F + 2E-3 + 0.3E-3), b = cms.double(b_L_1_F + 17E-3), c = cms.double(c_L_1_F + 11E-3 - 90E-3)),
          cms.PSet(unit=cms.string("R_1_F"), a = cms.double(a_R_1_F - 2E-3 + 1.3E-3), b = cms.double(b_R_1_F -  3E-3), c = cms.double(c_R_1_F + 30E-3 - 90E-3)),
          cms.PSet(unit=cms.string("R_2_F"), a = cms.double(a_R_2_F - 2E-3 - 1.3E-3), b = cms.double(b_R_2_F +  3E-3), c = cms.double(c_R_2_F - 30E-3 - 90E-3))
        )
      )
    ),

    cut1_a = 1., cut1_c = -2.0E-6, cut1_si = 35E-6,
    cut2_a = 1., cut2_c = -0.3E-6, cut2_si = 8E-6,

    cut5_a = 0.105, cut5_c = -0.01, cut5_si = 0.08,
    cut6_a = 0.105, cut6_c = +0.03, cut6_si = 0.08,

    cut7_a = 0., cut7_c = +0.01, cut7_si = 0.55,
    cut8_a = 0., cut8_c = -0.11, cut8_si = 2.8,

    cut9_a = -0.28, cut9_c = +0.00, cut9_si = 0.14,
    cut10_a = -0.29, cut10_c = +0.01, cut10_si = 0.14,

    fc_L = FiducialCut([[-350E-6, 31E-6], [50E-6, 31E-6], [250E-6, 40E-6], [390E-6, 80E-6], [270E-6, 126E-6], [-280E-6, 131E-6], [-390E-6, 60E-6]]),
    fc_R = FiducialCut([[-350E-6, 31E-6], [50E-6, 31E-6], [250E-6, 40E-6], [390E-6, 80E-6], [270E-6, 126E-6], [-280E-6, 131E-6], [-390E-6, 60E-6]]),
    fc_G = FiducialCut([[-338E-6, 32E-6], [50E-6, 32E-6], [250E-6, 42E-6], [385E-6, 80E-6], [270E-6, 124E-6], [-280E-6, 129E-6], [-385E-6, 60E-6]]),

    inefficiency_3outof4 = 0.0,
    inefficiency_pile_up = 0.0
  )
)

#----------------------------------------------------------------------------------------------------

cfg_45t_56b = cfg.clone(
  anal = dict(
    alignment_sources = cms.VPSet(
      cms.PSet(
        data = cms.VPSet(
          cms.PSet(unit=cms.string("L_2_F"), a = cms.double(a_L_2_F - 0.3E-3 + 1.5E-3), b = cms.double(b_L_2_F - 15E-3), c = cms.double(c_L_2_F + 15E-3 + 70E-3)),
          cms.PSet(unit=cms.string("L_1_F"), a = cms.double(a_L_1_F - 0.3E-3 - 1.5E-3), b = cms.double(b_L_1_F + 15E-3), c = cms.double(c_L_1_F - 15E-3 + 70E-3)),
          cms.PSet(unit=cms.string("R_1_F"), a = cms.double(a_R_1_F + 0.3E-3 + 1.3E-3), b = cms.double(b_R_1_F + 10E-3), c = cms.double(c_R_1_F + 20E-3 + 70E-3)),
          cms.PSet(unit=cms.string("R_2_F"), a = cms.double(a_R_2_F + 0.3E-3 - 1.3E-3), b = cms.double(b_R_2_F - 10E-3), c = cms.double(c_R_2_F - 20E-3 + 70E-3))
        )
      )
    ),

    cut1_a = 1., cut1_c = -0.5E-6, cut1_si = 34E-6,
    cut2_a = 1., cut2_c = -0.2E-6, cut2_si = 8E-6,

    cut5_a = 0.105, cut5_c = +0.01, cut5_si = 0.08,
    cut6_a = 0.105, cut6_c = -0.04, cut6_si = 0.08,

    cut7_a = 0., cut7_c = +0.00, cut7_si = 0.55,
    cut8_a = 0., cut8_c = 0.10, cut8_si = 2.8,

    cut9_a = -0.28, cut9_c = +0.01, cut9_si = 0.14,
    cut10_a = -0.29, cut10_c = -0.00, cut10_si = 0.14,

    fc_L = FiducialCut([[-50E-6, 31E-6], [-250E-6, 40E-6], [-370E-6, 80E-6], [-270E-6, 132E-6], [250E-6, 134E-6], [360E-6, 95E-6], [380E-6, 42E-6], [330E-6, 31E-6]]),
    fc_R = FiducialCut([[-50E-6, 31E-6], [-250E-6, 40E-6], [-370E-6, 80E-6], [-270E-6, 132E-6], [250E-6, 134E-6], [360E-6, 95E-6], [380E-6, 42E-6], [330E-6, 31E-6]]),
    fc_G = FiducialCut([[-50E-6, 32E-6], [-250E-6, 42E-6], [-365E-6, 80E-6], [-265E-6, 130E-6], [250E-6, 132E-6], [355E-6, 95E-6], [375E-6, 43E-6], [330E-6, 32E-6]]),

    inefficiency_3outof4 = 0.0,
    inefficiency_pile_up = 0.0
  )
)

#----------------------------------------------------------------------------------------------------

cfg_45b_56b = cfg_45t_56b.clone()

#----------------------------------------------------------------------------------------------------

cfg_45t_56t = cfg_45b_56t.clone()

#----------------------------------------------------------------------------------------------------

all_cfg = [cfg, cfg_45b_56t, cfg_45t_56b, cfg_45b_56b, cfg_45t_56t]
