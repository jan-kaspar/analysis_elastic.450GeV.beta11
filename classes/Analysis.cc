#include "Analysis.hh"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "common_event.hh"
#include "Kinematics.hh"
#include "common_alignment.hh"

#include <cmath>

//----------------------------------------------------------------------------------------------------

void Analysis::Load(const edm::ParameterSet &ps)
{
	for (const auto &p : ps.getParameter<vector<edm::ParameterSet>>("excl_timeIntervals"))
		excl_timeIntervals.emplace_back(p.getParameter<unsigned int>("first"), p.getParameter<unsigned int>("second"));

	excl_bunches = ps.getParameter<std::vector<unsigned int>>("excl_bunches");

	excl_runs = ps.getParameter<std::vector<unsigned int>>("excl_runs");

	for (const auto &p : ps.getParameter<vector<edm::ParameterSet>>("excl_lsIntervals"))
	{
		excl_lsIntervals[p.getParameter<unsigned int>("run")].push_back(pair<unsigned int, unsigned int>(p.getParameter<unsigned int>("ls_first"),
			p.getParameter<unsigned int>("ls_second")));
	}

	for (const auto &p : ps.getParameter<vector<edm::ParameterSet>>("alignment_sources"))
	{
		AlignmentSource src;
		src.Load(p.getParameter<vector<edm::ParameterSet>>("data"));
		alignment_sources.push_back(src);
	}

	t_min = ps.getParameter<double>("t_min");
	t_max = ps.getParameter<double>("t_max");
	t_min_full = ps.getParameter<double>("t_min_full");
	t_max_full = ps.getParameter<double>("t_max_full");
	t_min_fit = ps.getParameter<double>("t_min_fit");

	n_si = ps.getParameter<double>("n_si");

	cut1_a = ps.getParameter<double>("cut1_a");
	cut1_c = ps.getParameter<double>("cut1_c");
	cut1_si = ps.getParameter<double>("cut1_si");
	cut2_a = ps.getParameter<double>("cut2_a");
	cut2_c = ps.getParameter<double>("cut2_c");
	cut2_si = ps.getParameter<double>("cut2_si");
	cut3_a = ps.getParameter<double>("cut3_a");
	cut3_c = ps.getParameter<double>("cut3_c");
	cut3_si = ps.getParameter<double>("cut3_si");
	cut4_a = ps.getParameter<double>("cut4_a");
	cut4_c = ps.getParameter<double>("cut4_c");
	cut4_si = ps.getParameter<double>("cut4_si");
	cut5_a = ps.getParameter<double>("cut5_a");
	cut5_c = ps.getParameter<double>("cut5_c");
	cut5_si = ps.getParameter<double>("cut5_si");
	cut6_a = ps.getParameter<double>("cut6_a");
	cut6_c = ps.getParameter<double>("cut6_c");
	cut6_si = ps.getParameter<double>("cut6_si");
	cut7_a = ps.getParameter<double>("cut7_a");
	cut7_c = ps.getParameter<double>("cut7_c");
	cut7_si = ps.getParameter<double>("cut7_si");
	cut8_a = ps.getParameter<double>("cut8_a");
	cut8_c = ps.getParameter<double>("cut8_c");
	cut8_si = ps.getParameter<double>("cut8_si");
	cut9_a = ps.getParameter<double>("cut9_a");
	cut9_c = ps.getParameter<double>("cut9_c");
	cut9_si = ps.getParameter<double>("cut9_si");
	cut10_a = ps.getParameter<double>("cut10_a");
	cut10_c = ps.getParameter<double>("cut10_c");
	cut10_si = ps.getParameter<double>("cut10_si");

	fc_L.Init(ps.getParameter<vector<edm::ParameterSet>>("fc_L"));
	fc_R.Init(ps.getParameter<vector<edm::ParameterSet>>("fc_R"));
	fc_G.Init(ps.getParameter<vector<edm::ParameterSet>>("fc_G"));

	vtx_x_min = ps.getParameter<double>("vtx_x_min");
	vtx_x_max = ps.getParameter<double>("vtx_x_max");
	vtx_y_min = ps.getParameter<double>("vtx_y_min");
	vtx_y_max = ps.getParameter<double>("vtx_y_max");

	si_th_x_1arm_L = ps.getParameter<double>("si_th_x_1arm_L");
	si_th_x_1arm_R = ps.getParameter<double>("si_th_x_1arm_R");
	si_th_x_1arm_unc = ps.getParameter<double>("si_th_x_1arm_unc");
	si_th_x_2arm = ps.getParameter<double>("si_th_x_2arm");
	si_th_x_2arm_unc = ps.getParameter<double>("si_th_x_2arm_unc");
	si_th_x_LRdiff = ps.getParameter<double>("si_th_x_LRdiff");
	si_th_x_LRdiff_unc = ps.getParameter<double>("si_th_x_LRdiff_unc");

	si_th_y_1arm = ps.getParameter<double>("si_th_y_1arm");
	si_th_y_1arm_unc = ps.getParameter<double>("si_th_y_1arm_unc");
	si_th_y_2arm = ps.getParameter<double>("si_th_y_2arm");
	si_th_y_2arm_unc = ps.getParameter<double>("si_th_y_2arm_unc");
	si_th_y_LRdiff = ps.getParameter<double>("si_th_y_LRdiff");
	si_th_y_LRdiff_unc = ps.getParameter<double>("si_th_y_LRdiff_unc");

	use_resolution_fits = ps.getParameter<bool>("use_resolution_fits");
	use_3outof4_efficiency_fits = ps.getParameter<bool>("use_3outof4_efficiency_fits");
	use_pileup_efficiency_fits = ps.getParameter<bool>("use_pileup_efficiency_fits");

	inefficiency_3outof4 = ps.getParameter<double>("inefficiency_3outof4");
	inefficiency_shower_near = ps.getParameter<double>("inefficiency_shower_near");
	inefficiency_pile_up = ps.getParameter<double>("inefficiency_pile_up");
	inefficiency_trigger = ps.getParameter<double>("inefficiency_trigger");
	inefficiency_DAQ = ps.getParameter<double>("inefficiency_DAQ");

	bckg_corr = ps.getParameter<double>("bckg_corr");

	L_int = ps.getParameter<double>("L_int");

	eff_3outof4_fixed_point = ps.getParameter<double>("eff_3outof4_fixed_point");
	eff_3outof4_slope = ps.getParameter<double>("eff_3outof4_slope");
	eff_3outof4_slope_unc = ps.getParameter<double>("eff_3outof4_slope_unc");

	norm_corr = ps.getParameter<double>("norm_corr");
	norm_corr_unc = ps.getParameter<double>("norm_corr_unc");

	alignment_t0 = ps.getParameter<double>("alignment_t0");
	alignment_ts = ps.getParameter<double>("alignment_ts");

	binnings = ps.getParameter<vector<string>>("binnings");

	unsmearing_file = ps.getParameter<string>("unsmearing_file");
	unsmearing_object = ps.getParameter<string>("unsmearing_object");

	for (const auto &p : ps.getParameter<vector<edm::ParameterSet>>("alignment_y_ranges"))
	{
		alignmentYRanges[p.getParameter<string>("unit")] = AlignmentYRange(
			p.getParameter<double>("bot_min"),
			p.getParameter<double>("bot_max"),
			p.getParameter<double>("top_min"),
			p.getParameter<double>("top_max")
		);
	}
}

//----------------------------------------------------------------------------------------------------

void Analysis::Print() const
{
	printf("t_min=%E, t_max=%E, t_min_full=%E, t_max_full=%E\n", t_min, t_max, t_min_full, t_max_full);
	printf("t_min_fit=%E\n", t_min_fit);

	printf("\n");
	printf("%lu excluded runs: ", excl_runs.size());
	for (const auto &run : excl_runs)
		printf("%u, ", run);
	printf("\n");

	printf("%lu time intervals: ", excl_timeIntervals.size());
	for (const auto &interval : excl_timeIntervals)
		printf("(%u to %u), ", interval.first, interval.second);
	printf("\n");

	printf("exluded LS:\n");
	for (const auto &rp : excl_lsIntervals)
	{
		printf("    run %u: ", rp.first);
		for (const auto &interval : rp.second)
			printf("(%u to %u), ", interval.first, interval.second);
		printf("\n");
	}

	printf("%lu excluded bunches: ", excl_bunches.size());
	for (const auto &bunch : excl_bunches)
		printf("%u, ", bunch);
	printf("\n");

	printf("\n");
	printf("n_si=%E\n", n_si);

	printf("\n");
	printf("cut1_a=%E, cut1_c=%E, cut1_si=%E\n", cut1_a, cut1_c, cut1_si);
	printf("cut2_a=%E, cut2_c=%E, cut2_si=%E\n", cut2_a, cut2_c, cut2_si);
	printf("cut3_a=%E, cut3_c=%E, cut3_si=%E\n", cut3_a, cut3_c, cut3_si);
	printf("cut4_a=%E, cut4_c=%E, cut4_si=%E\n", cut4_a, cut4_c, cut4_si);
	printf("cut5_a=%E, cut5_c=%E, cut5_si=%E\n", cut5_a, cut5_c, cut5_si);
	printf("cut6_a=%E, cut6_c=%E, cut6_si=%E\n", cut6_a, cut6_c, cut6_si);
	printf("cut7_a=%E, cut7_c=%E, cut7_si=%E\n", cut7_a, cut7_c, cut7_si);
	printf("cut8_a=%E, cut8_c=%E, cut8_si=%E\n", cut8_a, cut8_c, cut8_si);
	printf("cut9_a=%E, cut9_c=%E, cut9_si=%E\n", cut9_a, cut9_c, cut9_si);
	printf("cut10_a=%E, cut10_c=%E, cut10_si=%E\n", cut10_a, cut10_c, cut10_si);

	printf("\n");
	printf("cut parameters:\n");
	for (unsigned int i = 1; i <= N_cuts; i++)
	{
		printf("%u| cqaN=%s, cqbN=%s | cca=%E, ccb=%E, ccc=%E, csi=%E\n", i,
			cqaN[i].c_str(), cqbN[i].c_str(), cca[i], ccb[i], ccc[i], csi[i]);
	}

	printf("\n");
	printf("%lu enabled cuts: ", cuts.size());
	for (unsigned int i = 0; i < cuts.size(); i++)
		printf((i == 0) ? "%i" : ", %i", cuts[i]);

	printf("\n");

	printf("\n");
	printf("fiducial cuts:\n");
	printf("fc_L: "); fc_L.Print();
	printf("fc_R: "); fc_R.Print();
	printf("fc_G: "); fc_G.Print();
	printf("vtx_x: min = %.3E, max = %.3E\n", vtx_x_min, vtx_x_max);
	printf("vtx_y: min = %.3E, max = %.3E\n", vtx_y_min, vtx_y_max);

	printf("\n");
	printf("smearing parameters:\n");
	printf("si_th_x_1arm_L=%E, si_th_x_1arm_R=%E, si_th_x_1arm_unc=%E\n", si_th_x_1arm_L, si_th_x_1arm_R, si_th_x_1arm_unc);
	printf("si_th_x_2arm=%E, si_th_x_2arm_unc=%E\n", si_th_x_2arm, si_th_x_2arm_unc);
	printf("si_th_x_LRDiff=%E, si_th_x_LRdiff_unc=%E\n", si_th_x_LRdiff, si_th_x_LRdiff_unc);
	printf("si_th_y_1arm=%E, si_th_y_1arm_unc=%E\n", si_th_y_1arm, si_th_y_1arm_unc);
	printf("si_th_y_2arm=%E, si_th_y_2arm_unc=%E\n", si_th_y_2arm, si_th_y_2arm_unc);
	printf("si_th_y_LRDiff=%E, si_th_y_LRdiff_unc=%E\n", si_th_y_LRdiff, si_th_y_LRdiff_unc);
	printf("use_resolution_fits = %i\n", use_resolution_fits);

	printf("\n");
	printf("normalisation parameters:\n");
	printf("use_3outof4_efficiency_fits = %i\n", use_3outof4_efficiency_fits);
	printf("use_pileup_efficiency_fits= %i\n", use_pileup_efficiency_fits);
	printf("inefficiency_3outof4 = %.3f\n", inefficiency_3outof4);
	printf("inefficiency_shower_near = %.3f\n", inefficiency_shower_near);
	printf("inefficiency_pile_up = %.3f\n", inefficiency_pile_up);
	printf("inefficiency_trigger = %.3f\n", inefficiency_trigger);
	printf("inefficiency_DAQ = %.3f\n", inefficiency_DAQ);
	printf("bckg_corr = %.3f\n", bckg_corr);
	printf("L_int=%E\n", L_int);
	printf("eff_3outof4_fixed_point=%E, eff_3outof4_slope=%E, eff_3outof4_slope_unc=%E\n", eff_3outof4_fixed_point, eff_3outof4_slope, eff_3outof4_slope_unc);
	printf("norm_corr=%E, norm_corr_unc=%E\n", norm_corr, norm_corr_unc);
}

//----------------------------------------------------------------------------------------------------

void Analysis::BuildCuts()
{
	N_cuts = 11;

	// cut structure:
	//	| a*qa + b*qb + c| < n_si * si

	cqaN[1] = "#theta_{x}^{R}"; cqbN[1] = "#theta_{x}^{L}";
	cca[1] = -cut1_a;
	ccb[1] = 1.;
	ccc[1] = cut1_c;
	csi[1] = cut1_si;
	cuts.push_back(1);
	
	cqaN[2] = "#theta_{y}^{R}"; cqbN[2] = "#theta_{y}^{L}";
	cca[2] = -cut2_a;
	ccb[2] = 1.;
	ccc[2] = cut2_c;
	csi[2] = cut2_si;
	cuts.push_back(2);

	cqaN[3] = "#theta_{x}^{R}"; cqbN[3] = "x^{R,2,F}";
	cca[3] = -cut3_a;
	ccb[3] = 1.;
	ccc[3] = cut3_c;
	csi[3] = cut3_si;
	//cuts.push_back(3);
	
	cqaN[4] = "#theta_{x}^{L}"; cqbN[4] = "x^{L,2,F}";
	cca[4] = -cut4_a;
	ccb[4] = 1.;
	ccc[4] = cut4_c;
	csi[4] = cut4_si;
	//cuts.push_back(4);
	
	cqaN[5] = "y^{R,2,F}"; cqbN[5] = "y^{R,2,F} - y^{R,1,F}";
	cca[5] = -cut5_a;
	ccb[5] = 1.;
	ccc[5] = cut5_c;
	csi[5] = cut5_si;
	cuts.push_back(5);
	
	cqaN[6] = "y^{L,2,F}"; cqbN[6] = "y^{L,2,F} - y^{L,1,F}";
	cca[6] = -cut6_a;
	ccb[6] = 1.;
	ccc[6] = cut6_c;
	csi[6] = cut6_si;
	cuts.push_back(6);
	
	cqaN[7] = "#theta_{x}"; cqbN[7] = "vtx_{x}^{R} - vtx_{x}^{L}";
	cca[7] = -cut7_a;
	ccb[7] = 1.;
	ccc[7] = cut7_c;
	csi[7] = cut7_si;
	cuts.push_back(7);
	
	cqaN[8] = "#theta_{y}"; cqbN[8] = "vtx_{y}^{R} - vtx_{y}^{L}";
	cca[8] = -cut8_a;
	ccb[8] = 1.;
	ccc[8] = cut8_c;
	csi[8] = cut8_si;
	cuts.push_back(8);
	
	cqaN[9] = "x^{R,2,F}"; cqbN[9] = "x^{R,2,F} - x^{R,1,F}";
	cca[9] = -cut9_a;
	ccb[9] = 1.;
	ccc[9] = cut9_c;
	csi[9] = cut9_si;
	cuts.push_back(9);
	
	cqaN[10] = "x^{L,2,F}"; cqbN[10] = "x^{L,2,F} - x^{L,1,F}";
	cca[10] = -cut10_a;
	ccb[10] = 1.;
	ccc[10] = cut10_c;
	csi[10] = cut10_si;
	cuts.push_back(10);

	// fake cut, only for tests
	cqaN[11] = "vtx_{x}"; cqbN[11] = "#theta_{x}^{R} - #theta_{x}^{L}";
	cca[11] = -5.24893e-05;
	ccb[11] = 1.;
	ccc[11] = 0.;
	csi[11] = 34E-6;
}

//----------------------------------------------------------------------------------------------------

bool Analysis::EvaluateCuts(const HitData & h, const Kinematics &k, CutData &cd) const
{
	cd.cqa[1] = k.th_x_R;	cd.cqb[1] = k.th_x_L;
	cd.cqa[2] = k.th_y_R;	cd.cqb[2] = k.th_y_L;
	
	cd.cqa[3] = k.th_x_R;	cd.cqb[3] = h.R_2_F.x;
	cd.cqa[4] = k.th_x_L;	cd.cqb[4] = h.L_2_F.x;

	cd.cqa[5] = h.R_2_F.y;	cd.cqb[5] = h.R_2_F.y - h.R_1_F.y;
	cd.cqa[6] = h.L_2_F.y;	cd.cqb[6] = h.L_2_F.y - h.L_1_F.y;

	cd.cqa[7] = k.th_x;		cd.cqb[7] = k.vtx_x_R - k.vtx_x_L;
	cd.cqa[8] = k.th_y;		cd.cqb[8] = k.vtx_y_R - k.vtx_y_L;

	cd.cqa[9] =  h.R_2_F.x;	cd.cqb[9] =  h.R_2_F.x - h.R_1_F.x;
	cd.cqa[10] = h.L_2_F.x;	cd.cqb[10] = h.L_2_F.x - h.L_1_F.x;

	cd.cqa[11] = k.vtx_x;	cd.cqb[11] = k.th_x_R - k.th_x_L;

	for (unsigned int ci = 1; ci <= N_cuts; ++ci)
	{
		cd.cv[ci] = cca[ci]*cd.cqa[ci] + ccb[ci]*cd.cqb[ci] + ccc[ci];
		cd.ct[ci] = (fabs(cd.cv[ci]) <= n_si * csi[ci]);
		//printf("cut %u: |%+E| < %E * %E <==> %i\n", ci, cd.cv[ci], n_si, csi[ci], cd.ct[ci]);
	}

	// and between all cuts
	bool select = true;	
	for (unsigned int ci = 0; ci < cuts.size(); ci++)
	{
		select &= cd.ct[cuts[ci]];
	}

	return select;
}

//----------------------------------------------------------------------------------------------------

bool Analysis::SkipEvent(unsigned int run, unsigned int ls, unsigned int timestamp, unsigned int bunch) const
{
	if (find(excl_runs.begin(), excl_runs.end(), run) != excl_runs.end())
		return true;

	for (const auto &interval : excl_timeIntervals)
	{
		if (interval.first <= timestamp && timestamp <= interval.second)
			return true;
	}

	const auto rit = excl_lsIntervals.find(run);
	if (rit != excl_lsIntervals.end())
	{
		for (const auto &interval : rit->second)
		{
			if (interval.first <= ls && ls <= interval.second)
				return true;
		}
	}

	if (find(excl_bunches.begin(), excl_bunches.end(), bunch) != excl_bunches.end())
		return true;

	return false;
}
