#include "AcceptanceCalculator.hh"

#include "Kinematics.hh"
#include "numerical_integration.hh"

//----------------------------------------------------------------------------------------------------

using std::make_unique;

void AcceptanceCalculator::Init(double _th_y_sign, const Analysis &_anal) {
  th_y_sign = _th_y_sign;
  anal = _anal;

  gaussianOptimisation = true;
  useSampledPhiFactor = false;

  integ_workspace_size_d_x = 1000;
  integ_workspace_d_x = gsl_integration_workspace_alloc(integ_workspace_size_d_x);

  integ_workspace_size_d_y = 1000;
  integ_workspace_d_y = gsl_integration_workspace_alloc(integ_workspace_size_d_y);

  integ_workspace_size_vtx_y = 1000;
  integ_workspace_vtx_y = gsl_integration_workspace_alloc(integ_workspace_size_vtx_y);

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

double AcceptanceCalculator::IntegOverVtxY(double x, double *par, const void* obj)
{
	// decode input
	const AcceptanceCalculator *ac = (AcceptanceCalculator *) obj;

	const double vtx_y = x;
	const double th = par[0];
	const double si_vtx_y = par[1];

	// weight
	const double w = exp(- vtx_y*vtx_y / 2. / si_vtx_y/si_vtx_y) / sqrt(2.*M_PI) / si_vtx_y;

	// calculate arc-length in within acceptance
	double phiSum = 0.;
	for (const auto &segment : ac->anal.fc_G.GetIntersectionPhis(th, vtx_y))
		phiSum += segment.second - segment.first;

	return w * phiSum;
}

//----------------------------------------------------------------------------------------------------

double AcceptanceCalculator::PhiFraction(double th) const
{
	const double si = anal.si_vtx_y;

	double par[2] = { th, si };

	const double result = RealIntegrate(AcceptanceCalculator::IntegOverVtxY, par, this, -6.*si, +6.*si, 0.0001, 0.,
		integ_workspace_size_vtx_y, integ_workspace_vtx_y, "AcceptanceCalculator::PhiFactor") / 2. / M_PI;

	return result;
}

//----------------------------------------------------------------------------------------------------

double AcceptanceCalculator::PhiFactor(double th) const
{
	if (useSampledPhiFactor)
		return 1. / s_phiFraction->Eval(th);
	else
		return 1. / PhiFraction(th);
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

	phi_corr = PhiFactor(k.th);

	return false;
}

//----------------------------------------------------------------------------------------------------

void AcceptanceCalculator::SamplePhiFactor()
{
	vector<double> x, y;

	for (double th = 200E-6; th < 700E-6; )
	{
		x.push_back(th);
		y.push_back(PhiFraction(th));

		double dth = 1E-6;

		if (th < 250E-6 || (th > 520E-6 && th < 530E-6) || th > 650E-6)
			dth = 0.1E-6;
		
		th += dth;
	}

    s_phiFraction = make_unique<TSpline3>("", x.data(), y.data(), x.size());

	useSampledPhiFactor = true;
}