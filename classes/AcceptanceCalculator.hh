#ifndef _AcceptanceCalculator_hh_
#define _AcceptanceCalculator_hh_

#include "Analysis.hh"

#include <gsl/gsl_integration.h>

//----------------------------------------------------------------------------------------------------

struct AcceptanceCalculator
{
	double th_y_sign;
	Analysis anal;

	bool gaussianOptimisation;

	unsigned long integ_workspace_size_d_x, integ_workspace_size_d_y;
	gsl_integration_workspace *integ_workspace_d_x, *integ_workspace_d_y;

	bool debug;

	void Init(double _th_y_sign, const Analysis &_anal)
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

	/// evaluates PDF of d_x, i.e. de_th_x_R - de_th_x_L
	double dist_d_x(double d_x) const;

	/// evaluates PDF of d_y
	double dist_d_y(double d_y) const;

	/// evaluates the acceptance condition
	double Condition(double th_x_p, double d_x, double th_y_p, double d_y) const;

	/// caculates the smearing integral over d_y
	static double IntegOverDY(double x, double *par, const void* obj);

	/// caculates the smearing integral over d_x
	static double IntegOverDX(double x, double *par, const void* obj);

	/// calculates the "smearing" component of the acceptance correction
	double SmearingFactor(double th_x_p, double th_y_p) const;

	/// returns true if event outside left, right fiducial cuts
	bool SmearingComponentCut(double th_x_L, double th_x_R, double th_y_L, double th_y_R) const;

	/// returns true if event outside global fiducial cuts
	bool PhiComponentCut(double th_x_p, double th_y_p) const;

	/// returns phi-acceptance factor
	double PhiFactor(double th) const;

	/// calculates the smearing corrections, for the event described by k
	/// returns flag whether the event should be skipped
	bool Calculate(const Kinematics &k, double &phi_corr, double &div_corr) const;
};

#endif
