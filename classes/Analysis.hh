#ifndef _Analysis_hh_
#define _Analysis_hh_

namespace edm
{
	class ParameterSet;
}

class HitData;
class CutData;
class Kinematics;

#include "common_alignment.hh"
#include "FiducialCut.hh"

#include <vector>
#include <map>
#include <string>

using namespace std;

//----------------------------------------------------------------------------------------------------

struct CutData
{
	double cqa[12];	///< array of quantities qa
	double cqb[12];	///< array of quantities qb
	double cv[12];	///< array of cut quantities v = a*qa + b*qb + c
	bool ct[12];	///< array of flags whether |v| < n_si * si
};

//----------------------------------------------------------------------------------------------------

struct Analysis
{
	// input selection
	std::vector<std::pair<unsigned int, unsigned int>> excl_timeIntervals;
	std::vector<unsigned int> excl_bunches;
	std::vector<unsigned int> excl_runs;
	std::map<unsigned int, std::vector<std::pair<unsigned int, unsigned int>>> excl_lsIntervals;

	// alignment corrections
	vector<AlignmentSource> alignment_sources;

	// binning, |t| in GeV^2
	double t_min, t_max;
	double t_min_full, t_max_full;
	double t_min_fit;

	// elastic selection cuts
	double n_si;

	double cut1_a, cut1_c, cut1_si;
	double cut2_a, cut2_c, cut2_si;
	double cut3_a, cut3_c, cut3_si;
	double cut4_a, cut4_c, cut4_si;
	double cut5_a, cut5_c, cut5_si;
	double cut6_a, cut6_c, cut6_si;
	double cut7_a, cut7_c, cut7_si;
	double cut8_a, cut8_c, cut8_si;
	double cut9_a, cut9_c, cut9_si;
	double cut10_a, cut10_c, cut10_si;

	// fiducial cuts
	FiducialCut fc_L, fc_R, fc_G;
	double vtx_x_min = -1E100, vtx_x_max = +1E100;
	double vtx_y_min = -1E100, vtx_y_max = +1E100;

	// (un)-smearing parameters
	double si_th_x_1arm_L;
	double si_th_x_1arm_R;
	double si_th_x_1arm_unc;
	double si_th_x_2arm;
	double si_th_x_2arm_unc;
	double si_th_x_LRdiff;
	double si_th_x_LRdiff_unc;

	double si_th_y_1arm;
	double si_th_y_1arm_unc;
	double si_th_y_2arm;
	double si_th_y_2arm_unc;
	double si_th_y_LRdiff;
	double si_th_y_LRdiff_unc;

	// efficiency parameters
	bool use_resolution_fits;				// whether to use time-dependent fits of resolution curves
	bool use_3outof4_efficiency_fits;		// whether to use time-dependent fits of 3-out-of-4 efficiency
	bool use_pileup_efficiency_fits;		// whether to use time-dependent fits of pile-up efficiency

	double inefficiency_3outof4;			// inefficiency from 3-out-of-4 method, used only if use_3outof4_efficiency_fits=false
	double inefficiency_shower_near;		// inefficiency due to shower in near RP
	double inefficiency_pile_up;			// inefficiency due to pile-up, used only if use_pileup_efficiency_fits=false
	double inefficiency_trigger;			// trigger inefficiency
	double inefficiency_DAQ;				// DAQ inefficiency

	// normalisation correction to subtract background
	double bckg_corr;

	// (delivered) luminosity
	double L_int;	// mb^-1

	// 3-out-of-4 efficiency uncertainty (only used in MC simulation)
	double eff_3outof4_fixed_point, eff_3outof4_slope, eff_3outof4_slope_unc;

	// normalisation correction and its uncertainty (only used in MC simulation)
	double norm_corr, norm_corr_unc;

	double alignment_t0;	// beginning of the first time-slice
	double alignment_ts;	// time-slice in s

	vector<string> binnings;	// default list of binnings

	string unsmearing_file;		// file with unsmearing corrections
	string unsmearing_object;	// specification of the unsmearing correction

	// y ranges for alignment
	struct AlignmentYRange
	{
		double bot_min, bot_max, top_min, top_max;
		AlignmentYRange(double bmi=0., double bma=0., double tmi=0., double tma=0.) :
			bot_min(bmi), bot_max(bma), top_min(tmi), top_max(tma) {}
	};
	map<std::string, AlignmentYRange> alignmentYRanges;

	void Print() const;

	void Load(const edm::ParameterSet &ps);

	unsigned int N_cuts;	// number of cuts - indexed from 1!
	string cqaN[12], cqbN[12];
	double cca[12], ccb[12], ccc[12], csi[12];
	std::vector<unsigned int> cuts;	// list of active cuts
	void BuildCuts();

	bool EvaluateCuts(const HitData &, const Kinematics &, CutData &) const;

	bool SkipEvent(unsigned int run, unsigned int ls, unsigned int timestamp, unsigned int bunch) const;
};

#endif
