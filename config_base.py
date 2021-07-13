import FWCore.ParameterSet.Config as cms
import math

#----------------------------------------------------------------------------------------------------

def FiducialCut(points, slope_pos_def = 0, slope_neg_def = 0):
  c = cms.VPSet()
  for p in points:
    slope_pos = slope_pos_def
    slope_neg = slope_neg_def

    if slope_pos * slope_neg < 0 and p[1] > 300E-6:
      slope_pos = -slope_pos_def
      slope_neg = -slope_neg_def

    if len(p) > 2:
      slope_pos = p[2]
      slope_neg = p[3]
    c.append(cms.PSet(x=cms.double(p[0]), y=cms.double(p[1]), slope_pos=cms.double(slope_pos), slope_neg=cms.double(slope_neg)))
  return c

#----------------------------------------------------------------------------------------------------

def Contour(fc):
  output = []
  for p in fc:
    output.append([p.x.pythonValue(), p.y.pythonValue(), p.slope_pos.pythonValue(), p.slope_neg.pythonValue()])
  return output

#----------------------------------------------------------------------------------------------------

def Shrink(points, sigma_x = 25E-6, sigma_y = 25E-6, th_x_low = -300E-6, th_x_high = +300E-6, th_y_low = 300E-6, th_y_high = 400E-6):
  output = []
  for p in points:
    x = p[0]
    y = p[1]

    if (x < th_x_low):
      x += sigma_x

    if (x > th_x_high):
      x -= sigma_x

    if (y < th_y_low):
      y += sigma_y

    if (y > th_y_high):
      y -= sigma_y

    pc = p[:]  # trick to make a copy
    pc[0] = x
    pc[1] = y

    output.append(pc)

  return output

#----------------------------------------------------------------------------------------------------

def CutContour(points, x1, y1, x2, y2):
  dx = x2 - x1
  dy = y2 - y1

  nx = -dy
  ny = dx

  output = []

  for i in range(len(points)):
    j = (i + 1) % len(points)

    pi_x = points[i][0]
    pi_y = points[i][1]

    pj_x = points[j][0]
    pj_y = points[j][1]

    pi_proj = (pi_x - x1) * nx + (pi_y - y1) * ny
    pj_proj = (pj_x - x1) * nx + (pj_y - y1) * ny

    pi_in = (pi_proj >= 0)
    pj_in = (pj_proj >= 0)

    if (pi_in and pj_in):
      output.append(points[j])

    if ((pi_in and (not pj_in)) or ((not pi_in) and pj_in)):
      # calculate intersection
      d2x = pj_x - pi_x
      d2y = pj_y - pi_y

      de_a_x = pi_x - x1
      de_a_y = pi_y - y1

      det = - dx * d2y + d2x * dy

      la1 = (-d2y * de_a_x + d2x * de_a_y) / det

      c_x = x1 + la1 * dx
      c_y = y1 + la1 * dy

      if not pi_in:
        output.append([c_x, c_y])
        output.append(points[j])
      else:
        output.append([c_x, c_y])

  return output

#----------------------------------------------------------------------------------------------------

cfg = cms.PSet(
  input_files = cms.vstring(),
  distilled_files = cms.vstring("."),

  timestamp0 = cms.double(1539468000),

  timestamp_min = cms.double(0.),
  timestamp_max = cms.double(0.),

  timber_dir = cms.string(""),

  env = cms.PSet(
    optics_version = cms.string("v3")
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

    si_vtx_y = cms.double(0.),
    si_vtx_y_LRdiff = cms.double(0.),

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

    binnings = cms.vstring("sb1", "sb2", "sb3"),

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

cfg.anal.si_vtx_y = 0.186
cfg.anal.si_vtx_y_LRdiff = 0.330

#----------------------------------------------------------------------------------------------------

contour_45b_56t_L = [[-200e-6, 550e-6], [-500e-6, 500e-6], [-560e-6, 220e-6], [-400e-6, 170e-6], [+200e-6, 170e-6], [+440e-6, 260e-6], [+370e-6, 500e-6], [+340e-6, 550e-6]]
contour_45b_56t_R = CutContour(contour_45b_56t_L, -1, 200E-6, +1, 200E-6)

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

    fc_L = FiducialCut(contour_45b_56t_L, -145E-6, -145E-6),
    fc_R = FiducialCut(contour_45b_56t_R, +164E-6, +164E-6),
    fc_G = FiducialCut(Shrink(contour_45b_56t_R), +164E-6, -145E-6)
  )
)

#----------------------------------------------------------------------------------------------------

contour_45t_56b_L = [[-200e-6, 170e-6], [+400e-6, 170e-6], [+520e-6, 200e-6], [+400e-6, 480e-6], [-430e-6, 480e-6], [-550e-6, 400e-6], [-600e-6, 200e-6]]
contour_45t_56b_R = CutContour(contour_45t_56b_L, -1, 200E-6, +1, 200E-6)

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

    fc_L = FiducialCut(contour_45t_56b_L, +145E-6, +145E-6),
    fc_R = FiducialCut(contour_45t_56b_R, -164E-6, -164E-6),
    fc_G = FiducialCut(Shrink(contour_45t_56b_R), +164E-6, -145E-6)
  )
)

#----------------------------------------------------------------------------------------------------

cfg_45b_56b = cfg_45t_56b.clone()

#----------------------------------------------------------------------------------------------------

cfg_45t_56t = cfg_45b_56t.clone()

#----------------------------------------------------------------------------------------------------

all_cfg = [cfg, cfg_45b_56t, cfg_45t_56b, cfg_45b_56b, cfg_45t_56t]
