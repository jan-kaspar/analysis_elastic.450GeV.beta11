#ifndef _Environment_hh_
#define _Environment_hh_

namespace edm
{
	class ParameterSet;
}

#include "TVectorD.h"
#include "TMatrixD.h"

using namespace std;

//----------------------------------------------------------------------------------------------------

struct Environment
{
	// beam momentum (GeV)
	double p, p_L, p_R;

	// beam momentum uncertainty
	double si_de_p;

	// beam divergence
	double si_th_x_L, si_th_y_L;		// rad
	double si_th_x_R, si_th_y_R;		// rad

	// TODO: used ?
	//double si_th_y_RL_assym_unc;		// uncertainty of the L-R assymetry

	// vertex smearing
	double si_vtx_x, si_vtx_y; 		// mm

	// pitch-induced error (i.e. spatial resolution)
	double si_de_P_L, si_de_P_R;	// mm

	// optics
	bool use_matched_optics;
	double v_x_L_1_F, v_x_L_2_F, v_x_R_1_F, v_x_R_2_F;	// 1
	double v_y_L_1_F, v_y_L_2_F, v_y_R_1_F, v_y_R_2_F;	// 1
	double L_x_L_1_F, L_x_L_2_F, L_x_R_1_F, L_x_R_2_F;	// mm
	double L_y_L_1_F, L_y_L_2_F, L_y_R_1_F, L_y_R_2_F;	// mm
	double D_x_L_1_F, D_x_L_2_F, D_x_R_1_F, D_x_R_2_F;	// mm

	// optics perturbation covariance matrices
	// order of elements:
	//		left arm:  v_x_L_N, L_x_L_N, v_y_L_N, L_y_L_N, v_x_L_F, L_x_L_F, v_y_L_F, L_y_L_F
	//		right arm: v_x_R_N, L_x_R_N, v_y_R_N, L_y_R_N, v_x_R_F, L_x_R_F, v_y_R_F, L_y_R_F
	// units: v's in 1, L's in m
	TMatrixDSym opt_cov;

	// optics perturbation generator matrices
	TMatrixD opt_per_gen;

	// alignment uncertainties
	double si_de_x, si_de_y;

	// misalignments (mm)
	// TODO: used ?
	/*
	double de_x_L_N, de_y_L_N, tilt_L_N;
	double de_x_L_F, de_y_L_F, tilt_L_F;
	double de_x_R_N, de_y_R_N, tilt_R_N;
	double de_x_R_F, de_y_R_F, tilt_R_F;
	*/

	Environment() : opt_cov(16), opt_per_gen(16, 16)
	{
	}

	void InitNominal();
	void UseMatchedOptics();

	void Load(const edm::ParameterSet &ps);

	void Print() const;

	void ApplyRandomOpticsPerturbations(TVectorD &de);

	void ApplyRandomOpticsPerturbations()
	{
		TVectorD de(16);
		ApplyRandomOpticsPerturbations(de);
	}

	/// modes counted from 0 to 15
	void ApplyOpticsPerturbationMode(int mode, double coef);

	/// modes counted from 0 to 7
	void ApplyEffectiveLengthPerturbationMode(int mode, double coef);
};

#endif
