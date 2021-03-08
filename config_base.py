import FWCore.ParameterSet.Config as cms
import math

#----------------------------------------------------------------------------------------------------

def FiducialCut(points, slope_def = 0):
  c = cms.VPSet()
  for p in points:
    slope = slope_def
    if len(p) > 2:
      slope = p[2]
    c.append(cms.PSet(x=cms.double(p[0]), y=cms.double(p[1]), slope=cms.double(slope)))
  return c

#----------------------------------------------------------------------------------------------------

cfg = cms.PSet(
  input_files = cms.vstring(),
  distilled_files = cms.vstring("."),

  timestamp0 = cms.double(1539468000),

  timestamp_min = cms.double(0.),
  timestamp_max = cms.double(0.),

  timber_dir = cms.string(""),

  env = cms.PSet(
    use_matched_optics = cms.bool(True)
  ),

  anal = cms.PSet(
    excl_timeIntervals = cms.VPSet(
      # fill 7302
      cms.PSet(first=cms.uint32(73400), second=cms.uint32(73500)), # high-PU prob
      cms.PSet(first=cms.uint32(76800), second=cms.uint32(76820)), # rapid rate decrease at the fill end
    ),

    excl_bunches = cms.vuint32(),

    excl_runs = cms.vuint32(),

    excl_lsIntervals = cms.VPSet(),

    alignment_sources = cms.VPSet(),

    t_min = cms.double(0.),
    t_max = cms.double(0.10),
    t_min_full = cms.double(0.),
    t_max_full = cms.double(0.15),
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
    alignment_ts = cms.double(15*60),

    binnings = cms.vstring("ub", "eb"),

    unsmearing_file = cms.string("unfolding_cf_ni_<diagonal>.root"),
    unsmearing_object = cms.string("fit-1/<binning>"),

    alignment_y_ranges = cms.VPSet(
      cms.PSet(unit=cms.string("L_2_F"), bot_min=cms.double(-11.5), bot_max=cms.double(-7.0), top_min=cms.double(+6.5), top_max=cms.double(+11.0)),
      cms.PSet(unit=cms.string("L_1_F"), bot_min=cms.double(-11.5), bot_max=cms.double(-7.0), top_min=cms.double(+6.5), top_max=cms.double(+11.0)),
      cms.PSet(unit=cms.string("R_1_F"), bot_min=cms.double( -9.5), bot_max=cms.double(-5.5), top_min=cms.double(+5.0), top_max=cms.double( +9.5)),
      cms.PSet(unit=cms.string("R_2_F"), bot_min=cms.double( -9.5), bot_max=cms.double(-5.0), top_min=cms.double(+5.0), top_max=cms.double( +9.5))
    )
  )
)

si_th_y_1arm = 32E-6
cfg.anal.si_th_y_1arm = si_th_y_1arm
cfg.anal.si_th_y_1arm_unc = -999.

cfg.anal.si_th_y_LRdiff = si_th_y_1arm * math.sqrt(2.)
cfg.anal.si_th_y_LRdiff_unc = -999.

cfg.anal.si_th_y_2arm = si_th_y_1arm / math.sqrt(2.)
cfg.anal.si_th_y_2arm_unc = -999.

si_th_x_1arm = 37E-6
cfg.anal.si_th_x_1arm_L = si_th_x_1arm
cfg.anal.si_th_x_1arm_R = si_th_x_1arm
cfg.anal.si_th_x_1arm_unc = -999.

cfg.anal.si_th_x_LRdiff = si_th_x_1arm * math.sqrt(2.)
cfg.anal.si_th_x_LRdiff_unc = -999.

cfg.anal.si_th_x_2arm = si_th_x_1arm / math.sqrt(2.)
cfg.anal.si_th_x_2arm_unc = -999.

#----------------------------------------------------------------------------------------------------

cfg_45b_56t = cfg.clone(
  anal = dict(
    cut1_a = 1., cut1_si = 55E-6,
	cut2_a = 1., cut2_si = 48E-6,

	cut5_a = -0.0186, cut5_si = 0.085,
	cut6_a = +0.0096, cut6_si = 0.085,

	cut7_a = 0., cut7_si = 0.14,
	cut8_a = 0., cut8_si = 0.35,

	cut9_a = -0.499, cut9_si = 0.22,
	cut10_a = -0.564, cut10_si = 0.26,

    # TODO: jsut test
    fc_L = FiducialCut([[-200e-6, 490e-6], [-500e-6, 450e-6], [-600e-6, 220e-6], [-400e-6, 160e-6], [+200e-6, 160e-6], [+500e-6, 220e-6], [+400e-6, 490e-6]]), #, -145E-6),
    fc_R = FiducialCut([[-200e-6, 490e-6], [-500e-6, 450e-6], [-600e-6, 220e-6], [-400e-6, 160e-6], [+200e-6, 160e-6], [+500e-6, 220e-6], [+400e-6, 490e-6]]), #, +164E-6),
    fc_G = FiducialCut([[-200e-6, 480e-6], [-480e-6, 440e-6], [-580e-6, 225e-6], [-400e-6, 170e-6], [+200e-6, 170e-6], [+480e-6, 225e-6], [+380e-6, 480e-6]])
  )
)

#----------------------------------------------------------------------------------------------------

cfg_45t_56b = cfg.clone(
  anal = dict(
	cut1_a = 1., cut1_si = 55E-6,
	cut2_a = 1., cut2_si = 48E-6,

	cut5_a = -0.0185, cut5_si = 0.085,
	cut6_a = +0.0068, cut6_si = 0.085,

	cut7_a = 0., cut7_si = 0.14,
	cut8_a = 0., cut8_si = 0.35,

	cut9_a = -0.491, cut9_si = 0.22,
	cut10_a = -0.541, cut10_si = 0.26,

    fc_L = FiducialCut([[-200e-6, 170e-6], [+400e-6, 170e-6], [+520e-6, 200e-6], [+400e-6, 480e-6], [-430e-6, 480e-6], [-550e-6, 400e-6], [-600e-6, 200e-6]]),
    fc_R = FiducialCut([[-200e-6, 170e-6], [+400e-6, 170e-6], [+520e-6, 200e-6], [+400e-6, 480e-6], [-430e-6, 480e-6], [-550e-6, 400e-6], [-600e-6, 200e-6]]),
    fc_G = FiducialCut([[-200e-6, 180e-6], [+400e-6, 180e-6], [+500e-6, 205e-6], [+380e-6, 470e-6], [-420e-6, 470e-6], [-530e-6, 400e-6], [-580e-6, 210e-6]])
  )
)

#----------------------------------------------------------------------------------------------------

cfg_45b_56b = cfg_45t_56b.clone()

#----------------------------------------------------------------------------------------------------

cfg_45t_56t = cfg_45b_56t.clone()

#----------------------------------------------------------------------------------------------------

all_cfg = [cfg, cfg_45b_56t, cfg_45t_56b, cfg_45b_56b, cfg_45t_56t]
