#include <string>
#include <vector>
#include <map>
#include <cmath>

double timestamp0 = 1539468000;

vector<string> distilledNtuples;

vector<AlignmentSource> alignmentSources;
Analysis anal;
Environment env;

string unsmearing_file;
string unsmearing_object;

//----------------------------------------------------------------------------------------------------

void Init_global()
{
	// list of (sub-)directories with distilled ntuples
	distilledNtuples.push_back(".");

	// input selection
	/*
	anal.excl_runs.push_back(324467);

	anal.excl_lsIntervals[324462] = { {94, 338} };
	anal.excl_lsIntervals[324532] = { {399, 520} };
	anal.excl_lsIntervals[324536] = { {360, 509} };

	anal.excl_timeIntervals.push_back({112100, 112400});
	anal.excl_timeIntervals.push_back({115000, 117000});
	*/

	// environment settings
	env.InitNominal();

	// binning
	// TODO
	anal.t_min = 0.; anal.t_max = 0.2;
	anal.t_min_full = 0.; anal.t_max_full = 0.3;

	// approximate (time independent) resolutions
	// TODO
	anal.si_th_y_1arm = 15.7E-6;
	anal.si_th_y_1arm_unc = 0.;

	anal.si_th_y_LRdiff = anal.si_th_y_1arm * sqrt(2.);
	anal.si_th_y_LRdiff_unc = -999.;

	anal.si_th_y_2arm = 0.;
	anal.si_th_y_2arm_unc = -999.;

	anal.si_th_x_1arm_L = 0E-6;
	anal.si_th_x_1arm_R = anal.si_th_x_1arm_L;
	anal.si_th_x_1arm_unc = -999.;

	anal.si_th_x_LRdiff = anal.si_th_x_1arm_L * sqrt(2.);
	anal.si_th_x_LRdiff_unc = -999.;

	anal.si_th_x_2arm = 0E-6;
	anal.si_th_x_2arm_unc = -999.;

	// analysis settings
	anal.use_resolution_fits = true;

	anal.use_3outof4_efficiency_fits = false;
	anal.use_pileup_efficiency_fits = false;
	anal.inefficiency_3outof4 = 0.;				// diagonal dependent
	anal.inefficiency_shower_near = 0.03;
	anal.inefficiency_pile_up = 0.;				// diagonal dependent
	anal.inefficiency_trigger = 0.;

	anal.bckg_corr = 1.;

	anal.L_int = 0.;	// mb^-1, diagonal dependent

	anal.t_min_fit = 0.; // TODO

	anal.alignment_t0 = 0.;	// beginning of the first time-slice
	anal.alignment_ts = 30.*60.;	// time-slice in s

	anal.alignmentYRanges["L_2_F"] = Analysis::AlignmentYRange(-11.5, -7.0, +6.5, +11.0);
	anal.alignmentYRanges["L_1_F"] = Analysis::AlignmentYRange(-11.5, -7.0, +6.5, +11.0);
	anal.alignmentYRanges["R_1_F"] = Analysis::AlignmentYRange(-9.5, -5.0, +5.0, +9.5);
	anal.alignmentYRanges["R_2_F"] = Analysis::AlignmentYRange(-9.5, -5.0, +5.0, +9.5);

	unsmearing_file = "";	// diagonal dependent
	unsmearing_object = "fit-1/<binning>";
}

//----------------------------------------------------------------------------------------------------

void Init_global_45b_56t()
{
	// crude alignment
	AlignmentSource alSrc;
	alSrc.SetAlignmentA(atConstant);
	alSrc.SetAlignmentB(atConstant);
	alSrc.SetAlignmentC(atConstant);

	alSrc.cnst.a_L_2_F =  -8E-3; alSrc.cnst.b_L_2_F = +300E-3; alSrc.cnst.c_L_2_F =   +0E-3 - 350E-3;
	alSrc.cnst.a_L_1_F = -10E-3; alSrc.cnst.b_L_1_F = +700E-3; alSrc.cnst.c_L_1_F = -200E-3 - 350E-3;
                                                                       
	alSrc.cnst.a_R_1_F =  -6E-3; alSrc.cnst.b_R_1_F = -300E-3; alSrc.cnst.c_R_1_F = -200E-3 - 350E-3;
	alSrc.cnst.a_R_2_F =  -6E-3; alSrc.cnst.b_R_2_F = +500E-3; alSrc.cnst.c_R_2_F = -100E-3 - 350E-3;
	
	alignmentSources.push_back(alSrc);

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = +41E-6; anal.cut1_si = 55E-6;
	anal.cut2_a = 1.; anal.cut2_c =  0E-6; anal.cut2_si = 48E-6;

	anal.cut5_a = -0.0186; anal.cut5_c = -0.0; anal.cut5_si = 0.085;
	anal.cut6_a = +0.0096; anal.cut6_c = -0.08; anal.cut6_si = 0.085;

	anal.cut7_a = +1370.; anal.cut7_c = +0.16; anal.cut7_si = 0.12;
	anal.cut8_a = -3067.; anal.cut8_c = -0.45; anal.cut8_si = 0.37;

	anal.cut9_a = -0.499; anal.cut9_c = -0.07; anal.cut9_si = 0.22;
	anal.cut10_a = -0.564; anal.cut10_c = +0.13; anal.cut10_si = 0.26;

	anal.fc_L = FiducialCut({{-400E-6, 200E-6}, {+400E-6, 200E-6}, {+450E-6, 250E-6}, {+400E-6, 510E-6}, {-400E-6, 510E-6}, {-450E-6, 250E-6}});
	anal.fc_R = FiducialCut({{-400E-6, 200E-6}, {+400E-6, 200E-6}, {+450E-6, 250E-6}, {+400E-6, 510E-6}, {-400E-6, 510E-6}, {-450E-6, 250E-6}});
	anal.fc_G = FiducialCut({{-400E-6, 210E-6}, {+400E-6, 210E-6}, {+440E-6, 250E-6}, {+400E-6, 500E-6}, {-400E-6, 500E-6}, {-440E-6, 250E-6}});

	unsmearing_file = "unfolding_cf_ni_45b_56t.root";

	anal.inefficiency_3outof4 = 0.0; // TODO
	anal.inefficiency_pile_up = 0.0; // TODO
}

//----------------------------------------------------------------------------------------------------

void Init_global_45t_56b()
{
	// crude alignment
	AlignmentSource alSrc;
	alSrc.SetAlignmentA(atConstant);
	alSrc.SetAlignmentB(atConstant);
	alSrc.SetAlignmentC(atConstant);

	alSrc.cnst.a_L_2_F =  -8E-3; alSrc.cnst.b_L_2_F = +300E-3; alSrc.cnst.c_L_2_F =   +0E-3 - 500E-3;
	alSrc.cnst.a_L_1_F = -10E-3; alSrc.cnst.b_L_1_F = +700E-3; alSrc.cnst.c_L_1_F = -200E-3 - 500E-3;
                                                                                                    
	alSrc.cnst.a_R_1_F =  -6E-3; alSrc.cnst.b_R_1_F = -300E-3; alSrc.cnst.c_R_1_F = -200E-3 - 500E-3;
	alSrc.cnst.a_R_2_F =  -6E-3; alSrc.cnst.b_R_2_F = +500E-3; alSrc.cnst.c_R_2_F = -100E-3 - 500E-3;

	alignmentSources.push_back(alSrc);

	// analysis settings
	anal.cut1_a = 1.; anal.cut1_c = +34E-6; anal.cut1_si = 55E-6;
	anal.cut2_a = 1.; anal.cut2_c =   0E-6; anal.cut2_si = 48E-6;

	anal.cut5_a = -0.0185; anal.cut5_c = -0.03; anal.cut5_si = 0.085;
	anal.cut6_a = +0.0068; anal.cut6_c = -0.08; anal.cut6_si = 0.085;

	anal.cut7_a = +1308.; anal.cut7_c = +0.17; anal.cut7_si = 0.12;
	anal.cut8_a = -3057.; anal.cut8_c = +0.09; anal.cut8_si = 0.37;

	anal.cut9_a = -0.491; anal.cut9_c = -0.05; anal.cut9_si = 0.22;
	anal.cut10_a = -0.541; anal.cut10_c = +0.12; anal.cut10_si = 0.26;

	anal.fc_L = FiducialCut({{-400E-6, 200E-6}, {+400E-6, 200E-6}, {+450E-6, 250E-6}, {+400E-6, 510E-6}, {-400E-6, 510E-6}, {-450E-6, 250E-6}});
	anal.fc_R = FiducialCut({{-400E-6, 200E-6}, {+400E-6, 200E-6}, {+450E-6, 250E-6}, {+400E-6, 510E-6}, {-400E-6, 510E-6}, {-450E-6, 250E-6}});
	anal.fc_G = FiducialCut({{-400E-6, 210E-6}, {+400E-6, 210E-6}, {+440E-6, 250E-6}, {+400E-6, 500E-6}, {-400E-6, 500E-6}, {-440E-6, 250E-6}});

	unsmearing_file = "unfolding_cf_ni_45t_56b.root";

	anal.inefficiency_3outof4 = 0.0; // TODO
	anal.inefficiency_pile_up = 0.0; // TODO
}
