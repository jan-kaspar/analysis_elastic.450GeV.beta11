#include "AcceptanceCalculator.hh"

#include "Kinematics.hh"
#include "numerical_integration.hh"

//----------------------------------------------------------------------------------------------------

void AcceptanceCalculator::Init(double _th_y_sign, const Analysis &_anal)
{
	th_y_sign = _th_y_sign;
	anal = _anal;

	gaussianOptimisation = true;

	integ_workspace_size_d_x = 1000;
	integ_workspace_d_x = gsl_integration_workspace_alloc(integ_workspace_size_d_x);

	integ_workspace_size_d_y = 1000;
	integ_workspace_d_y = gsl_integration_workspace_alloc(integ_workspace_size_d_y);

	debug = false;
}

//----------------------------------------------------------------------------------------------------

double AcceptanceCalculator::dist_d_x(double d_x) const
{
	const double si_d_x = anal.si_th_x_LRdiff;
	const double r = d_x / si_d_x;
	return exp(-r*r/2.) / sqrt(2. * M_PI) / si_d_x;
}

//----------------------------------------------------------------------------------------------------

double AcceptanceCalculator::dist_d_y(double d_y) const
{
	const double si_d_y = anal.si_th_y_LRdiff;
	const double r = d_y / si_d_y;
	return exp(-r*r/2.) / sqrt(2. * M_PI) / si_d_y;
}

//----------------------------------------------------------------------------------------------------

double AcceptanceCalculator::Condition(double th_x_p, double d_x, double th_y_p, double d_y, double vtx_y_L_p, double vtx_y_R_p) const
{
	const double th_x_p_R = th_x_p + d_x/2.;
	const double th_x_p_L = th_x_p - d_x/2.;

	const double th_y_p_R = th_y_p + d_y/2.;
	const double th_y_p_L = th_y_p - d_y/2.;

	if (anal.fc_L.Satisfied(th_x_p_L, th_y_p_L, vtx_y_L_p) && anal.fc_R.Satisfied(th_x_p_R, th_y_p_R, vtx_y_R_p))
		return 1.;

	return 0;
}

//----------------------------------------------------------------------------------------------------

double AcceptanceCalculator::IntegOverDY(double d_y, double *, const void* obj)
{
	AcceptanceCalculator *ac = (AcceptanceCalculator *) obj;

	return ac->dist_d_y(d_y);
}

//----------------------------------------------------------------------------------------------------

double AcceptanceCalculator::IntegOverDX(double x, double *par, const void* obj)
{
	const double &d_x = x;

	const double &th_x_p = par[0];
	const double &th_y_p = par[1];
	const double &vtx_y_L_p = par[2];
	const double &vtx_y_R_p = par[3];

	AcceptanceCalculator *ac = (AcceptanceCalculator *) obj;

	if (ac->debug)
		printf("    d_x = %E\n", d_x);

	const double th_x_p_R = th_x_p + d_x/2.;
	const double th_x_p_L = th_x_p - d_x/2.;

	const auto [th_y_L_cut_l, th_y_L_cut_h] = ac->anal.fc_L.GetThYRange(th_x_p_L, vtx_y_L_p);
	const auto [th_y_R_cut_l, th_y_R_cut_h] = ac->anal.fc_R.GetThYRange(th_x_p_R, vtx_y_R_p);

	if (ac->debug)
		printf("         th_y_L_cut_l = %E, th_y_R_cut_l = %E\n", th_y_L_cut_l, th_y_R_cut_l);

	double th_y_abs = ac->th_y_sign * th_y_p;

	double d_y_min = 2. * max(th_y_R_cut_l - th_y_abs, th_y_abs - th_y_L_cut_h);
	double d_y_max = 2. * min(th_y_R_cut_h - th_y_abs, th_y_abs - th_y_L_cut_l);

	if (d_y_min >= d_y_max)
		return 0;

	double I = 0.;

	if (ac->gaussianOptimisation)
	{
		I = ( TMath::Erf(d_y_max / sqrt(2.) / ac->anal.si_th_y_LRdiff) - TMath::Erf(d_y_min / sqrt(2.) / ac->anal.si_th_y_LRdiff) ) / 2.;
	} else {
		I = RealIntegrate(AcceptanceCalculator::IntegOverDY, nullptr, ac, d_y_min, d_y_max, 0., 1E-3,
			ac->integ_workspace_size_d_y, ac->integ_workspace_d_y, "AcceptanceCalculator::IntegOverDX");
	}

	if (ac->debug)
		printf("         dist = %E, I = %E\n", ac->dist_d_x(d_x), I);

	return ac->dist_d_x(d_x) * I;
}

//----------------------------------------------------------------------------------------------------

double AcceptanceCalculator::SmearingFactor(double th_x_p, double th_y_p, double vtx_y_L_p, double vtx_y_R_p) const
{
	double par[4] = { th_x_p, th_y_p, vtx_y_L_p, vtx_y_R_p };

	const double si_d_x = anal.si_th_x_LRdiff;

	return RealIntegrate(AcceptanceCalculator::IntegOverDX, par, this, -6.*si_d_x, +6.*si_d_x, 0.001, 0.,
		integ_workspace_size_d_x, integ_workspace_d_x, "AcceptanceCalculator::SmearingFactor");
}

//----------------------------------------------------------------------------------------------------

bool AcceptanceCalculator::SmearingComponentCut(double th_x_L, double th_x_R, double th_y_L, double th_y_R, double vtx_y_L_p, double vtx_y_R_p) const
{
	return (!anal.fc_L.Satisfied(th_x_L, th_y_sign * th_y_L, vtx_y_L_p) || !anal.fc_R.Satisfied(th_x_R, th_y_sign * th_y_R, vtx_y_R_p));
}

//----------------------------------------------------------------------------------------------------

bool AcceptanceCalculator::PhiComponentCut(double th_x, double th_y, double vtx_y) const
{
	return (!anal.fc_G.Satisfied(th_x, th_y_sign * th_y, vtx_y));
}

//----------------------------------------------------------------------------------------------------

double AcceptanceCalculator::PhiFactor(double th, double vtx_y) const
{
	// calculate arc-length in within acceptance
	double phiSum = 0.;
	for (const auto &segment : anal.fc_G.GetIntersectionPhis(th, vtx_y))
		phiSum += segment.second - segment.first;

	return 2. * M_PI / phiSum;
}

//----------------------------------------------------------------------------------------------------

bool AcceptanceCalculator::Calculate(const Kinematics &k, double &phi_corr, double &div_corr) const
{
	// ----- smearing component, cut -----

	if (SmearingComponentCut(k.th_x_L, k.th_x_R, k.th_y_L, k.th_y_R, k.vtx_y_L, k.vtx_y_R))
		return true;

	// ----- phi component, cut -----

	if (PhiComponentCut(k.th_x, k.th_y, k.vtx_y))
		return true;

	// ----- smearing component, correction -----

	const double F = SmearingFactor(k.th_x, k.th_y, k.vtx_y_L, k.vtx_y_R);
	div_corr = 1. / F;

	// ----- phi component, correction -----

	phi_corr = PhiFactor(k.th, k.vtx_y);

	return false;
}
