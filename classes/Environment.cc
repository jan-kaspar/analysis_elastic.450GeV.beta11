#include "Environment.hh"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "TMatrixDSymEigen.h"
#include "TRandom3.h"

//----------------------------------------------------------------------------------------------------

void Environment::InitNominal()
{
	// beam momentum (GeV)
	p = p_L = p_R = 450.;

	// momentum uncertainty
	si_de_p = 1E-3 * p;

	// beam divergence (rad, for simulation)
	si_th_x_L = si_th_x_R = 6.7E-6;	// beta*_x = 70 m
	si_th_y_L = si_th_y_R = 5.6E-6; // beta*_y = 100 m

	// vertex smearing (mm, for simulation)
	si_vtx_x = 330E-3;
	si_vtx_y = 395E-3;

	// pitch-induced error (mm)
	si_de_P_L = si_de_P_R = 12E-3;

	// optics (nominal, sent by Frici on 26 Feb 2020)
	v_x_R_1_F = -2.29251110442821; L_x_R_1_F = 35.1828162653735E3; v_y_R_1_F = 0.126220649033512; L_y_R_1_F = 171.911207102938E3; D_x_R_1_F = -0.0864671103458512E3;
	v_x_R_2_F = -1.97644302902099; L_x_R_2_F = 27.2787476702851E3; v_y_R_2_F = 0.100459146707445; L_y_R_2_F = 192.282747401847E3; D_x_R_2_F = -0.0683872276311071E3;

	v_x_L_1_F = -2.24947329593445; L_x_L_1_F = 35.8475945504415E3; v_y_L_1_F = 0.124161170232800; L_y_L_1_F = 173.630053228259E3; D_x_L_1_F = +0.0855111365318488E3;
	v_x_L_2_F = -1.93196310652978; L_x_L_2_F = 27.6759142870506E3; v_y_L_2_F = 0.098769215019425; L_y_L_2_F = 194.499649253117E3; D_x_L_2_F = +0.0672035864182922E3;

	// optics imperfections, real numbers not yet available
	/*
	double opt_cov_data[] = {
		1.66491785322919E-5,	7.89369350809322E-4,	-6.32104648991575E-5,	-2.59256651347955E-3,	1.32082198894547E-5,	6.74825862436010E-4,	-7.05099468507492E-5,	-2.90814857624182E-3,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,
		7.89369350809322E-4,	4.25168512900734E-2,	-2.69123774586626E-3,	-1.27194879518425E-1,	6.22063175217557E-4,	3.68812664207966E-2,	-3.00284426127789E-3,	-1.43008843891627E-1,   0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,
		-6.32104648991575E-5,	-2.69123774586626E-3,	3.87268155124952E-4,	1.16710801015928E-2,	-5.29597981974794E-5,	-2.04304942253293E-3,	4.31885407514716E-4,	1.29371751873752E-2,    0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,
		-2.59256651347955E-3,	-1.27194879518425E-1,	1.16710801015928E-2,	4.67495352905620E-1,	-2.06850163729353E-3,	-1.07850543630469E-1,	1.30191344764728E-2,	5.23596681087111E-1,    0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,
		1.32082198894547E-5,	6.22063175217557E-4,	-5.29597981974794E-5,	-2.06850163729353E-3,	1.05589617320310E-5,	5.24577953037806E-4,	-5.90732823172670E-5,	-2.31625003829467E-3,   0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,
		6.74825862436010E-4,	3.68812664207966E-2,	-2.04304942253293E-3,	-1.07850543630469E-1,	5.24577953037806E-4,	3.26401174262052E-2,	-2.27990513284437E-3,	-1.21628367533737E-1,   0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,
		-7.05099468507492E-5,	-3.00284426127789E-3,	4.31885407514716E-4,	1.30191344764728E-2,	-5.90732823172670E-5,	-2.27990513284437E-3,	4.81643176886755E-4,	1.44316029530475E-2,    0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,
		-2.90814857624182E-3,	-1.43008843891627E-1,	1.29371751873752E-2,	5.23596681087111E-1,	-2.31625003829467E-3,	-1.21628367533737E-1,	1.44316029530475E-2,	5.86636930463780E-1,    0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,

		0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,		1.70596E-005,	7.58302E-004,	-6.32105E-005,	-2.54045E-003,	1.37330E-005,	6.34320E-004,	-7.05009E-005,	-2.84011E-003,
		0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	    7.58302E-004,	4.53036E-002,	-2.69559E-003,	-1.31790E-001,	5.81606E-004,	4.04788E-002,	-3.00795E-003,	-1.48951E-001,
		0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	    -6.32105E-005,	-2.69559E-003,	3.87058E-004,	1.16688E-002,	-5.29702E-005,	-2.04924E-003,	4.31625E-004,	1.29347E-002,
		0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	    -2.54045E-003,	-1.31790E-001,	1.16688E-002,	4.74781E-001,	-2.00147E-003,	-1.13824E-001,	1.30165E-002,	5.33061E-001,
		0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	    1.37330E-005,	5.81606E-004,	-5.29702E-005,	-2.00147E-003,	1.12292E-005,	4.71804E-004,	-5.90750E-005,	-2.22875E-003,
		0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	    6.34320E-004,	4.04788E-002,	-2.04924E-003,	-1.13824E-001,	4.71804E-004,	3.72829E-002,	-2.28723E-003,	-1.29352E-001,
		0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	    -7.05009E-005,	-3.00795E-003,	4.31625E-004,	1.30165E-002,	-5.90750E-005,	-2.28723E-003,	4.81325E-004,	1.44289E-002,
		0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	0.0000000000E+00,	    -2.84011E-003,	-1.48951E-001,	1.29347E-002,	5.33061E-001,	-2.22875E-003,	-1.29352E-001,	1.44289E-002,	5.98935E-001
	};
	opt_cov.SetMatrixArray(opt_cov_data);

	TMatrixDSymEigen eig_decomp(opt_cov);
	TVectorD eig_values(eig_decomp.GetEigenValues());
	TMatrixDSym S(16);
	for (unsigned int i = 0; i < 16; i++)
		S(i, i) = (eig_values(i) >= 0.) ? sqrt(eig_values(i)) : 0.;
	opt_per_gen = eig_decomp.GetEigenVectors() * S;
	*/

	// alignment uncertainties
	// TODO: check if used, move to python?
	/*
	si_de_x = 0E-3;
	si_de_y_R = 0E-3;
	si_de_y_D = 0E-3;
	si_tilt = 0E-3;
	*/

	// other uncertainties
	// TODO: check if used, move to python?
	//si_th_y_RL_assym_unc = 0.;
}

//----------------------------------------------------------------------------------------------------

void Environment::UseMatchedOptics()
{
	printf(">> Environment::UseMatchedOptics\n");
	
	throw "No matched optics available";
}

//----------------------------------------------------------------------------------------------------

void Environment::Load(const edm::ParameterSet &ps)
{
	InitNominal();

	use_matched_optics = ps.getParameter<bool>("use_matched_optics");

	if (use_matched_optics)
		UseMatchedOptics();
}

//----------------------------------------------------------------------------------------------------

void Environment::Print() const
{
	printf("p=%E, p_L=%E, p_R=%E\n", p, p_L, p_R);
	printf("\n");
	printf("si_th_x_L=%E, si_th_y_L=%E\n", si_th_x_L, si_th_y_L);
	printf("si_th_x_R=%E, si_th_y_R=%E\n", si_th_x_R, si_th_y_R);
	printf("si_vtx_x=%E, si_vtx_y=%E\n", si_vtx_x, si_vtx_y);
	printf("si_de_P_L=%E, si_de_P_R=%E\n", si_de_P_L, si_de_P_R);
	printf("\n");

	printf("L_x_L_1_F = %E, v_x_L_1_F = %E, L_y_L_1_F = %E, v_y_L_1_F = %E\n", L_x_L_1_F, v_x_L_1_F, L_y_L_1_F, v_y_L_1_F);
	printf("L_x_L_2_F = %E, v_x_L_2_F = %E, L_y_L_2_F = %E, v_y_L_2_F = %E\n", L_x_L_2_F, v_x_L_2_F, L_y_L_2_F, v_y_L_2_F);
	printf("L_x_R_1_F = %E, v_x_R_1_F = %E, L_y_R_1_F = %E, v_y_R_1_F = %E\n", L_x_R_1_F, v_x_R_1_F, L_y_R_1_F, v_y_R_1_F);
	printf("L_x_R_2_F = %E, v_x_R_2_F = %E, L_y_R_2_F = %E, v_y_R_2_F = %E\n", L_x_R_2_F, v_x_R_2_F, L_y_R_2_F, v_y_R_2_F);

	// TODO
	/*
	printf("\n");
	printf("si_de_x=%E, si_de_y_R=%E, si_de_y_D=%E, si_tilt=%E\n", si_de_x, si_de_y_R, si_de_y_D, si_tilt);
	printf("\n");
	printf("de_x_L_N=%E, de_y_L_N=%E, tilt_L_N=%E\n", de_x_L_N, de_y_L_N, tilt_L_N);
	printf("de_x_L_F=%E, de_y_L_F=%E, tilt_L_F=%E\n", de_x_L_F, de_y_L_F, tilt_L_F);
	printf("de_x_R_N=%E, de_y_R_N=%E, tilt_R_N=%E\n", de_x_R_N, de_y_R_N, tilt_R_N);
	printf("de_x_R_F=%E, de_y_R_F=%E, tilt_R_F=%E\n", de_x_R_F, de_y_R_F, tilt_R_F);
	printf("\n");
	printf("si_th_y_RL_assym_unc=%E\n", si_th_y_RL_assym_unc);
	*/

	printf("optics uncertainties: left arm\n");
	printf("\tv_x_N: %.4f\n", sqrt(opt_cov(0, 0)));
	printf("\tL_x_N: %.4f m\n", sqrt(opt_cov(1, 1)));
	printf("\tv_y_N: %.4f\n", sqrt(opt_cov(2, 2)));
	printf("\tL_y_N: %.4f m\n", sqrt(opt_cov(3, 3)));
	printf("\tv_x_F: %.4f\n", sqrt(opt_cov(4, 4)));
	printf("\tL_x_F: %.4f m\n", sqrt(opt_cov(5, 5)));
	printf("\tv_y_F: %.4f\n", sqrt(opt_cov(6, 6)));
	printf("\tL_y_F: %.4f m\n", sqrt(opt_cov(7, 7)));

	printf("optics uncertainties: right arm\n");
	printf("\tv_x_N: %.4f\n", sqrt(opt_cov(8, 8)));
	printf("\tL_x_N: %.4f m\n", sqrt(opt_cov(9, 9)));
	printf("\tv_y_N: %.4f\n", sqrt(opt_cov(10, 10)));
	printf("\tL_y_N: %.4f m\n", sqrt(opt_cov(11, 11)));
	printf("\tv_x_F: %.4f\n", sqrt(opt_cov(12, 12)));
	printf("\tL_x_F: %.4f m\n", sqrt(opt_cov(13, 13)));
	printf("\tv_y_F: %.4f\n", sqrt(opt_cov(14, 14)));
	printf("\tL_y_F: %.4f m\n", sqrt(opt_cov(15, 15)));
}

//----------------------------------------------------------------------------------------------------

void Environment::ApplyRandomOpticsPerturbations(TVectorD & de)
{
	printf("Environment::ApplyRandomOpticsPerturbations: not yet implemented\n");

	TVectorD r(16);

	for (unsigned int i = 0; i < 16; i++)
		r(i) = gRandom->Gaus();
	de = opt_per_gen * r;

	v_x_L_1_F += de(0) * 1E0;
	L_x_L_1_F += de(1) * 1E3;
	v_y_L_1_F += de(2) * 1E0;
	L_y_L_1_F += de(3) * 1E3;
	v_x_L_2_F += de(4) * 1E0;
	L_x_L_2_F += de(5) * 1E3;
	v_y_L_2_F += de(6) * 1E0;
	L_y_L_2_F += de(7) * 1E3;

	v_x_R_1_F += de(8) * 1E0;
	L_x_R_1_F += de(9) * 1E3;
	v_y_R_1_F += de(10) * 1E0;
	L_y_R_1_F += de(11) * 1E3;
	v_x_R_2_F += de(12) * 1E0;
	L_x_R_2_F += de(13) * 1E3;
	v_y_R_2_F += de(14) * 1E0;
	L_y_R_2_F += de(15) * 1E3;
}

//----------------------------------------------------------------------------------------------------

void Environment::ApplyOpticsPerturbationMode(int mode, double coef)
{
	printf("Environment::ApplyOpticsPerturbationMode: not yet implemented\n");

	printf(">> Environment::ApplyOpticsPerturbationMode\n");

	// prepare correlation matrix
	TMatrixDSym cor(opt_cov);
	TMatrixDSym Sigma(opt_cov);
	for (int i = 0; i < opt_cov.GetNrows(); i++)
		for (int j = 0; j < opt_cov.GetNcols(); j++)
		{
			cor(i, j) /= sqrt( opt_cov(i, i) * opt_cov(j, j) );
			Sigma(i, j) = (i == j) ? sqrt( opt_cov(i, i) ) : 0.;
		}

	// eigen decomposition
	TMatrixDSymEigen eig_decomp(cor);
	TVectorD eig_values(eig_decomp.GetEigenValues());

	// construct mode
	TVectorD vm(opt_cov.GetNrows());
	for (int i = 0; i < opt_cov.GetNrows(); i++)
	{
		double l = eig_values(i);
		double sl = (l > 0.) ? sqrt(l) : 0.;
		vm(i) = (i == mode) ? sl * coef : 0.;
	}

	vm = Sigma * eig_decomp.GetEigenVectors() * vm;

	printf("\tleft arm: mode %u, coefficient %+.3f\n", mode, coef);
	vm.Print();

	v_x_L_1_F += vm(0) * 1E0;
	L_x_L_1_F += vm(1) * 1E3;
	v_y_L_1_F += vm(2) * 1E0;
	L_y_L_1_F += vm(3) * 1E3;
	v_x_L_2_F += vm(4) * 1E0;
	L_x_L_2_F += vm(5) * 1E3;
	v_y_L_2_F += vm(6) * 1E0;
	L_y_L_2_F += vm(7) * 1E3;

	v_x_R_1_F += vm(8) * 1E0;
	L_x_R_1_F += vm(9) * 1E3;
	v_y_R_1_F += vm(10) * 1E0;
	L_y_R_1_F += vm(11) * 1E3;
	v_x_R_2_F += vm(12) * 1E0;
	L_x_R_2_F += vm(13) * 1E3;
	v_y_R_2_F += vm(14) * 1E0;
	L_y_R_2_F += vm(15) * 1E3;
}

//----------------------------------------------------------------------------------------------------

void Environment::ApplyEffectiveLengthPerturbationMode(int mode, double coef)
{
	printf(">> Environment::ApplyEffectiveLengthPerturbationMode\n");

	// prepare reduced covariance matrix
	TMatrixDSym cov_red(8);
	for (unsigned int i = 0; i < 8; i++)
		for (unsigned int j = 0; j < 8; j++)
			cov_red(i, j) = opt_cov(2*i+1, 2*j+1);

	// eigen decomposition
	TMatrixDSymEigen eig_decomp(cov_red);
	TVectorD eig_values(eig_decomp.GetEigenValues());

	// construct mode
	TVectorD vm(cov_red.GetNrows());
	for (int i = 0; i < cov_red.GetNrows(); i++)
	{
		double l = eig_values(i);
		double sl = (l > 0.) ? sqrt(l) : 0.;
		vm(i) = (i == mode) ? sl * coef : 0.;
	}

	vm = eig_decomp.GetEigenVectors() * vm;

	printf("\tmode %u, coefficient %+.3f\n", mode, coef);
	//vm.Print();

	L_x_L_1_F += vm(0) * 1E3;
	L_y_L_1_F += vm(1) * 1E3;
	L_x_L_2_F += vm(2) * 1E3;
	L_y_L_2_F += vm(3) * 1E3;

	L_x_R_1_F += vm(4) * 1E3;
	L_y_R_1_F += vm(5) * 1E3;
	L_x_R_2_F += vm(6) * 1E3;
	L_y_R_2_F += vm(7) * 1E3;
}
