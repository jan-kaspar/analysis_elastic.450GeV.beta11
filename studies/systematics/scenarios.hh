#ifndef _scenarios_hh_
#define _scenarios_hh_

#include "classes/Environment.hh"
#include "classes/Analysis.hh"
#include "classes/common_init.hh"

#include "TF1.h"
#include "TFile.h"

#include <vector>
#include <string>

using namespace std;

//----------------------------------------------------------------------------------------------------

struct BiasesPerArm
{
	// shifts (rad)
	//    th_y_reco = th_y_true + sh_th_y
	double sh_th_x = 0., sh_th_y = 0.;

	// tilts (rad)
	//    th_x_reco = th_x_true + tilt_th_x_eff_prop_to_th_y * th_y
	//    th_y_reco = th_y_true + tilt_th_y_eff_prop_to_th_x * th_x
	double tilt_th_x_eff_prop_to_th_y = 0.;
	double tilt_th_y_eff_prop_to_th_x = 0.;

	// scales
	//    th_y_reco = sc_th_y * th_y_true
	double sc_th_x = 1., sc_th_y = 1.;
};

//----------------------------------------------------------------------------------------------------

struct Biases
{
	// per-arm biases
	BiasesPerArm L, R;

	// errors of the inefficiency corrections (3/4 and 2/4)
	double eff_intercept = 0.;
	double eff_slope = 0.;	// rad^-1

	// normalisation error (relative factor)
	double norm = 0.;

	// non-gaussian distributions
	bool use_non_gaussian_d_x = false;
	bool use_non_gaussian_d_y = false;

	// d-m correlation
	double d_m_corr_coef_x = 0.;
	double d_m_corr_coef_y = 0.;

	void Print() const
	{
		printf("left arm:\n");
		printf("    sh_th_x = %.3E, sh_th_y = %.3E\n", L.sh_th_x, L.sh_th_y);
		printf("    tilt_th_x_eff_prop_to_th_y = %.3E, tilt_th_y_eff_prop_to_th_x = %.3E\n", L.tilt_th_x_eff_prop_to_th_y, L.tilt_th_y_eff_prop_to_th_x);
		printf("    sc_th_x = %.3E, sc_th_y = %.3E\n", L.sc_th_x, L.sc_th_y);

		printf("right arm:\n");
		printf("    sh_th_x = %.3E, sh_th_y = %.3E\n", R.sh_th_x, R.sh_th_y);
		printf("    tilt_th_x_eff_prop_to_th_y = %.3E, tilt_th_y_eff_prop_to_th_x = %.3E\n", R.tilt_th_x_eff_prop_to_th_y, R.tilt_th_y_eff_prop_to_th_x);
		printf("    sc_th_x = %.3E, sc_th_y = %.3E\n", R.sc_th_x, R.sc_th_y);

		printf("global:\n");
		printf("    eff_intercept = %.3E, eff_slope = %.3E\n", eff_intercept, eff_slope);
		printf("    norm = %.3E\n", norm);
		printf("    use_non_gaussian_d_x = %u\n", use_non_gaussian_d_x);
		printf("    use_non_gaussian_d_y = %u\n", use_non_gaussian_d_y);
		printf("    d_m_corr_coef_x = %.3f\n", d_m_corr_coef_x);
		printf("    d_m_corr_coef_y = %.3f\n", d_m_corr_coef_y);
	}
};

//----------------------------------------------------------------------------------------------------

int SetScenario(const string &scenario, Biases &biases, Environment & /*env_sim*/, Analysis &/*anal_sim*/, Environment &env_rec, Analysis &anal_rec)
{
	if (scenario == "none")
	{
		return 0;
	}

	// ---------- theta_x shift ----------

	{
		// v = sigma that corresponds to modes (L, R) = (+1, +1) or (+1, -1)
		// here, conversion from single-arm sigma:
		const double v = 11E-6 / sqrt(2);

		if (scenario == "sh-thx")
		{
			biases.L.sh_th_x = v;
			biases.R.sh_th_x = v;

			anal_rec.fc_L.Shift(v, 0.);
			anal_rec.fc_R.Shift(v, 0.);
			anal_rec.fc_G.Shift(v, 0.);

			return 0;
		}

		if (scenario == "sh-thx-LRasym")
		{
			biases.L.sh_th_x = +v;
			biases.R.sh_th_x = -v;

			anal_rec.fc_L.Shift(+v, 0.);
			anal_rec.fc_R.Shift(-v, 0.);

			return 0;
		}
	}

	// ---------- y shift ----------

	{
		// sigma of the TB correlated, LR symmetric mode
		const double v = 20E-6 / sqrt(2);

		// sigma of the TB correlated, LR anti-symmetric mode
		const double v_LR_asym = 4E-6;

		// sigma for the TB uncorrelated modes (L, R) = (+1, +1) or (+1, -1)
		const double v_TB_uncorr = 10E-6 / sqrt(2.);

		if (scenario == "sh-thy")
		{
			biases.L.sh_th_y = v;
			biases.R.sh_th_y = v;

			anal_rec.fc_L.Shift(0., v * cfg.th_y_sign);
			anal_rec.fc_R.Shift(0., v * cfg.th_y_sign);
			anal_rec.fc_G.Shift(0., v * cfg.th_y_sign);

			return 0;
		}

		if (scenario == "sh-thy-LRasym")
		{
			biases.L.sh_th_y = +v_LR_asym;
			biases.R.sh_th_y = -v_LR_asym;

			anal_rec.fc_L.Shift(0., +v_LR_asym * cfg.th_y_sign);
			anal_rec.fc_R.Shift(0., -v_LR_asym * cfg.th_y_sign);

			return 0;
		}

		if (scenario == "sh-thy-TBuncor")
		{
			biases.L.sh_th_y = v_TB_uncorr;
			biases.R.sh_th_y = v_TB_uncorr;

			anal_rec.fc_L.Shift(0., v_TB_uncorr * cfg.th_y_sign);
			anal_rec.fc_R.Shift(0., v_TB_uncorr * cfg.th_y_sign);
			anal_rec.fc_G.Shift(0., v_TB_uncorr * cfg.th_y_sign);

			return 0;
		}

		if (scenario == "sh-thy-TBuncor-LRasym")
		{
			biases.L.sh_th_y = +v_TB_uncorr;
			biases.R.sh_th_y = -v_TB_uncorr;

			anal_rec.fc_L.Shift(0., +v_TB_uncorr * cfg.th_y_sign);
			anal_rec.fc_R.Shift(0., -v_TB_uncorr * cfg.th_y_sign);

			return 0;
		}
	}

	// ---------- xy tilt ----------

	{
		// v = sigma that corresponds to modes (L, R) = (+1, +1) or (+1, -1)
		// division by sqrt(2) is conversion from single-arm sigmas
		const double C = 0.02 / sqrt(2.);
		const double D = 0.01 / sqrt(2.);

		if (scenario.compare("tilt-thx-thy") == 0)
		{
			biases.L.tilt_th_x_eff_prop_to_th_y = C;
			biases.R.tilt_th_x_eff_prop_to_th_y = C;

			biases.L.tilt_th_y_eff_prop_to_th_x = D;
			biases.R.tilt_th_y_eff_prop_to_th_x = D;

			anal_rec.fc_L.ApplyCDTransform(C * cfg.th_y_sign, D * cfg.th_y_sign);
			anal_rec.fc_R.ApplyCDTransform(C * cfg.th_y_sign, D * cfg.th_y_sign);
			anal_rec.fc_G.ApplyCDTransform(C * cfg.th_y_sign, D * cfg.th_y_sign);

			return 0;
		}

		if (scenario.compare("tilt-thx-thy-LRasym") == 0)
		{
			biases.L.tilt_th_x_eff_prop_to_th_y = +C;
			biases.R.tilt_th_x_eff_prop_to_th_y = -C;

			biases.L.tilt_th_y_eff_prop_to_th_x = +D;
			biases.R.tilt_th_y_eff_prop_to_th_x = -D;

			anal_rec.fc_L.ApplyCDTransform(C * cfg.th_y_sign, D * cfg.th_y_sign);
			anal_rec.fc_R.ApplyCDTransform(-C * cfg.th_y_sign, -D * cfg.th_y_sign);
			// assume that the global contour is not modified

			return 0;
		}
	}

	// ---------- xy scaling (optics) ----------

	if (scenario.find("sc-thxy-mode") == 0)
	{
		const string mode = scenario.substr(12);

		double val_L_x=0., val_L_y, val_R_x, val_R_y;

		if (mode == "1") { val_L_x = -2.299E-02; val_R_x = -2.263E-02; val_L_y = +1.597E-02, val_R_y = +1.643E-02; }
		if (mode == "2") { val_L_x = -2.472E-04; val_R_x = -1.030E-02; val_L_y = -1.726E-02, val_R_y = +2.250E-03; }
		if (mode == "3") { val_L_x = +6.852E-03; val_R_x = +2.220E-03; val_L_y = +2.004E-04, val_R_y = +1.245E-02; }
		if (mode == "4") { val_L_x = -3.455E-03; val_R_x = +3.269E-03; val_L_y = -1.725E-03, val_R_y = +1.346E-03; }

		if (val_L_x != 0.)
		{
			biases.L.sc_th_x = 1. + val_L_x;
			biases.R.sc_th_x = 1. + val_R_x;
			biases.L.sc_th_y = 1. + val_L_y;
			biases.R.sc_th_y = 1. + val_R_y;

			anal_rec.fc_L.Scale(1. + val_L_x, 1. + val_L_y);
			anal_rec.fc_R.Scale(1. + val_R_x, 1. + val_R_y);
			anal_rec.fc_G.Scale(1. + (val_L_x + val_R_x)/2, 1. + (val_L_y + val_R_y)/2);

			return 0;
		}
	}

	// ---------- acceptance correction ----------

	/*
	if (scenario == "dx-sigma")
	{
		// TODO: update
		anal_sim.si_th_x_LRdiff += 0.3E-6;
		return 0;
	}

	if (scenario == "dy-sigma")
	{
		// TODO: update
		anal_sim.si_th_y_LRdiff += 0.007E-6;
		return 0;
	}

	if (scenario == "dx-non-gauss")
	{
		// TODO: update
		biases.use_non_gaussian_d_x = true;
		return 0;
	}

	if (scenario == "dx-mx-corr")
	{
		// TODO: update
		biases.d_m_corr_coef_x = +0.12;
		return 0;
	}

	if (scenario == "dy-my-corr")
	{
		// TODO: update
		biases.d_m_corr_coef_y = -0.26;
		return 0;
	}
	*/

	// ---------- inefficiency correction ----------

	/*
	if (scenario == "eff-intercept")
	{
		// TODO: update
		// combination from 3/4 (0.003) and 2/4 (0.01)
		biases.eff_intercept = sqrt(0.003*0.003 + 0.01*0.01);
		return 0;
	}

	if (scenario == "eff-slope")
	{
		// TODO: update
		biases.eff_slope = 15.;
		return 0;
	}
	*/

	// ---------- beam momentum ----------

	if (scenario.compare("beam-mom") == 0)
	{
		// The relative uncertainty of 10^-3 is the more conservative estimate.
		// NB: from Eq. (27) in https://cds.cern.ch/record/1546734 one may consider quoting the relative ucertainty
		// of 0.11 / 450 ~ 2.5 * 10^-4
		env_rec.p *= (1. - 0.001);

		return 0;
	}

	// ---------- unsmearing ----------

	/*
	if (scenario == "mx-sigma")
	{
		// TODO: update
		anal_sim.si_th_x_2arm += 0.04E-6;
		return 0;
	}

	if (scenario == "my-sigma")
	{
		// TODO: update
		anal_sim.si_th_y_2arm += 0.010E-6;
		return 0;
	}
	*/

	// ---------- normalisation ----------

	/*
	if (scenario == "norm")
	{
		// TODO: update
		biases.norm = 0.05;
		return 0;
	}
	*/

	return 1;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void CalculateNonGaussianDistributionTransformation(TF1 *dist, double min, double max, double sigma0,
	double &et, double &al, double &be)
{
	const unsigned int n_points = 1000;
	const double step = (max - min) / (n_points - 1);

	double I0=0., I1=0., I2=0.;
	for (unsigned int i = 0; i < n_points; i++)
	{
		double x = min + i * step;
		double f = dist->Eval(x);

		I0 += f * step;
		I1 += f*x * step;
		I2 += f*x*x * step;
	}

	et = sqrt((I2 - I1*I1/I0) / sigma0/sigma0 / I0/I0/I0);
	al = et * I0;
	be = I1 / I0;
}

//----------------------------------------------------------------------------------------------------

TF1 *f_non_gaussian_dist_d_x = nullptr;
double ngx_et, ngx_al, ngx_be;

void LoadNonGaussianDistributions(double si_d_x, double /* si_d_y */)
{
	// TODO: remove once functional
	throw 1;

	// TODO: update path
	TFile *f_in = TFile::Open("/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/beta2500/2rp/non-gaussianity/fit_dx.root");
	f_non_gaussian_dist_d_x = new TF1(* (TF1 *) f_in->Get("ff"));
	delete f_in;

	const double range = 6. * 15E-6;
	CalculateNonGaussianDistributionTransformation(f_non_gaussian_dist_d_x, -range, +range, si_d_x, ngx_et, ngx_al, ngx_be);
}

//----------------------------------------------------------------------------------------------------

double NonGauassianDistribution_d_x(double x)
{
	return f_non_gaussian_dist_d_x->Eval(ngx_al*(x + ngx_be)) * ngx_et;
}

#endif
