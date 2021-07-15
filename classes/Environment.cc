#include "Environment.hh"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "TMatrixDSymEigen.h"
#include "TRandom3.h"

//----------------------------------------------------------------------------------------------------

Environment::Environment() : optics_version("v1"), opt_cov(16), opt_per_gen(16, 16)
{
}

//----------------------------------------------------------------------------------------------------

void Environment::Init()
{
	// beam momentum (GeV)
	p = p_L = p_R = 450.;

	// momentum uncertainty
	si_de_p = 1E-3 * p;

	// beam divergence (rad)
	si_th_x_L = si_th_x_R = 16E-6;
	si_th_y_L = si_th_y_R = 16E-6;

	// vertex smearing (mm)
	si_vtx_x = 125E-3;
	si_vtx_y = 125E-3;

	// pitch-induced error (mm), later adjusted by parameters.h
	si_de_P_L = si_de_P_R = 12E-3;

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

	InitOptics();
}

//----------------------------------------------------------------------------------------------------

void Environment::InitOptics()
{
	bool optics_set = false;

	// default/empty optics imperfections
	array<double, n_optical_functions * n_optical_functions> opt_cov_data;
	for (int idx = 0; idx < n_optical_functions * n_optical_functions; ++idx)
		opt_cov_data[idx] = 0;

	// version nominal
	// zero uncertainties
	if (optics_version == "nominal")
	{
		v_x_R_1_F = -3.50435319019148; L_x_R_1_F = 9.63720339960312E3; v_y_R_1_F = -2.85401239131265; L_y_R_1_F = 18.1193542771580E3;
		v_x_R_2_F = -3.21469929522496; L_x_R_2_F = 6.84312044909302E3; v_y_R_2_F = -3.13198840555586; L_y_R_2_F = 17.4314616375356E3;

		v_x_L_1_F = -3.48842105181819; L_x_L_1_F = 9.19360885324731E3; v_y_L_1_F = -3.04102845358226; L_y_L_1_F = 20.9577511745132E3;
		v_x_L_2_F = -3.24137540851482; L_x_L_2_F = 6.53589039675599E3; v_y_L_2_F = -3.34416396613407; L_y_L_2_F = 20.7450069119535E3;

		optics_set = true;
	}

	// version 1, sent by Frici on 5 Feb 2021
	// zero uncertainties
	if (optics_version == "v1")
	{
		v_x_R_1_F = -3.52753964496120; L_x_R_1_F = 7.91175096826241E3; v_y_R_1_F = -2.75673027638370; L_y_R_1_F = 21.6645286702689E3;
		v_x_R_2_F = -3.23644508764819; L_x_R_2_F = 5.27795735088729E3; v_y_R_2_F = -3.02770849648774; L_y_R_2_F = 21.2592919586316E3;

		v_x_L_1_F = -3.51479293559723; L_x_L_1_F = 7.22927874910527E3; v_y_L_1_F = -2.89913772712385; L_y_L_1_F = 26.0546793411391E3;
		v_x_L_2_F = -3.26649342681412; L_x_L_2_F = 4.73246279442568E3; v_y_L_2_F = -3.19119325805915; L_y_L_2_F = 26.2715191647691E3;

		optics_set = true;
	}

	// version 2, sent by Frici on 17 Feb 2021
	// zero uncertainties
	if (optics_version == "v2")
	{
		v_x_R_1_F = -3.51892503698085; L_x_R_1_F = 7.68589490453928E3; v_y_R_1_F = -2.79059184045729; L_y_R_1_F = 21.1588449673039E3;
		v_x_R_2_F = -3.22610783144590; L_x_R_2_F = 5.05824818424874E3; v_y_R_2_F = -3.06617608982323; L_y_R_2_F = 20.7414119654111E3;

		v_x_L_1_F = -3.49562800456804; L_x_L_1_F = 6.87196655705152E3; v_y_L_1_F = -2.95460701403060; L_y_L_1_F = 25.1072739045313E3;
		v_x_L_2_F = -3.23890482648799; L_x_L_2_F = 4.36624713678055E3; v_y_L_2_F = -3.25591531830203; L_y_L_2_F = 25.3002472049893E3;

		optics_set = true;
	}

	// version 3, sent by Frici on 26 Feb 2021
	if (optics_version == "v3")
	{
		v_x_R_1_F = -3.37978981478134; L_x_R_1_F = 8.82282471441190E3; v_y_R_1_F = -2.99430086419815; L_y_R_1_F = 18.9452010171022E3;
		v_x_R_2_F = -3.04941084058860; L_x_R_2_F = 5.88738628407328E3; v_y_R_2_F = -3.31121674715817; L_y_R_2_F = 18.6104804831054E3;

		v_x_L_1_F = -3.36464565919960; L_x_L_1_F = 7.88529287571811E3; v_y_L_1_F = -3.11738584037042; L_y_L_1_F = 22.7372614679657E3;
		v_x_L_2_F = -3.06433535858017; L_x_L_2_F = 5.10041717075500E3; v_y_L_2_F = -3.45539504923435; L_y_L_2_F = 22.9564574706806E3;

		opt_cov_data = {
			1.00066E-003, 1.84705E-002, -7.29783E-004, -2.42089E-002, 1.30747E-003, 1.55418E-002, -8.99375E-004, -2.53098E-002, 4.55096E-004, 8.38117E-003, -1.05837E-004, -8.15930E-003, 6.75618E-004, 6.76829E-003, -1.70803E-004, -8.47889E-003,
			1.84705E-002, 5.34738E-001, -1.94023E-002, -6.65879E-001, 2.11557E-002, 4.73875E-001, -2.19298E-002, -7.09742E-001, -4.71867E-004, 1.48821E-001, 5.73812E-003, -1.11566E-001, 5.01317E-004, 1.35730E-001, 6.77860E-003, -1.29303E-001,
			-7.29783E-004, -1.94023E-002, 1.76052E-003, 3.19811E-002, -6.55318E-004, -1.75616E-002, 1.94878E-003, 3.36749E-002, 1.06292E-004, -3.68820E-003, 5.28259E-004, 8.72327E-003, 2.30982E-004, -3.84737E-003, 5.50473E-004, 9.29505E-003,
			-2.42089E-002, -6.65879E-001, 3.19811E-002, 9.16319E-001, -2.69145E-002, -5.90575E-001, 3.59990E-002, 9.73629E-001, 1.50064E-005, -1.82790E-001, -1.25248E-004, 1.92307E-001, -3.75870E-004, -1.68337E-001, -6.46407E-004, 2.13182E-001,
			1.30747E-003, 2.11557E-002, -6.55318E-004, -2.69145E-002, 1.80202E-003, 1.73145E-002, -8.72034E-004, -2.79595E-002, 7.81143E-004, 1.15868E-002, -1.39029E-004, -1.06704E-002, 1.16034E-003, 9.02136E-003, -2.54178E-004, -1.08976E-002,
			1.55418E-002, 4.73875E-001, -1.75616E-002, -5.90575E-001, 1.73145E-002, 4.22137E-001, -1.96474E-002, -6.30460E-001, -1.62581E-003, 1.23115E-001, 5.49927E-003, -9.04161E-002, -1.29920E-003, 1.14671E-001, 6.64352E-003, -1.06833E-001,
			-8.99375E-004, -2.19298E-002, 1.94878E-003, 3.59990E-002, -8.72034E-004, -1.96474E-002, 2.17521E-003, 3.78137E-002, 8.66897E-006, -4.98006E-003, 6.28840E-004, 1.05899E-002, 9.90859E-005, -4.86520E-003, 6.73841E-004, 1.11723E-002,
			-2.53098E-002, -7.09742E-001, 3.36749E-002, 9.73629E-001, -2.79595E-002, -6.30460E-001, 3.78137E-002, 1.03538E+000, 6.35429E-004, -1.91059E-001, -7.68596E-004, 1.97822E-001, 4.28127E-004, -1.77045E-001, -1.48085E-003, 2.20509E-001,
			4.55096E-004, -4.71867E-004, 1.06292E-004, 1.50064E-005, 7.81143E-004, -1.62581E-003, 8.66897E-006, 6.35429E-004, 7.56784E-004, 4.90816E-003, -3.99984E-004, -5.85990E-003, 1.06406E-003, 3.05087E-003, -5.52127E-004, -5.36972E-003,
			8.38117E-003, 1.48821E-001, -3.68820E-003, -1.82790E-001, 1.15868E-002, 1.23115E-001, -4.98006E-003, -1.91059E-001, 4.90816E-003, 7.95180E-002, -2.47223E-004, -6.52307E-002, 7.37048E-003, 6.27167E-002, -8.93176E-004, -6.70503E-002,
			-1.05837E-004, 5.73812E-003, 5.28259E-004, -1.25248E-004, -1.39029E-004, 5.49927E-003, 6.28840E-004, -7.68596E-004, -3.99984E-004, -2.47223E-004, 1.07774E-003, 7.04248E-003, -4.17802E-004, 1.85370E-004, 1.24822E-003, 6.43438E-003,
			-8.15930E-003, -1.11566E-001, 8.72327E-003, 1.92307E-001, -1.06704E-002, -9.04161E-002, 1.05899E-002, 1.97822E-001, -5.85990E-003, -6.52307E-002, 7.04248E-003, 1.07269E-001, -7.73405E-003, -5.00830E-002, 8.54627E-003, 1.06491E-001,
			6.75618E-004, 5.01317E-004, 2.30982E-004, -3.75870E-004, 1.16034E-003, -1.29920E-003, 9.90859E-005, 4.28127E-004, 1.06406E-003, 7.37048E-003, -4.17802E-004, -7.73405E-003, 1.52547E-003, 4.64550E-003, -6.18070E-004, -7.10997E-003,
			6.76829E-003, 1.35730E-001, -3.84737E-003, -1.68337E-001, 9.02136E-003, 1.14671E-001, -4.86520E-003, -1.77045E-001, 3.05087E-003, 6.27167E-002, 1.85370E-004, -5.00830E-002, 4.64550E-003, 5.11538E-002, -1.46830E-004, -5.25549E-002,
			-1.70803E-004, 6.77860E-003, 5.50473E-004, -6.46407E-004, -2.54178E-004, 6.64352E-003, 6.73841E-004, -1.48085E-003, -5.52127E-004, -8.93176E-004, 1.24822E-003, 8.54627E-003, -6.18070E-004, -1.46830E-004, 1.45948E-003, 7.79181E-003,
			-8.47889E-003, -1.29303E-001, 9.29505E-003, 2.13182E-001, -1.08976E-002, -1.06833E-001, 1.11723E-002, 2.20509E-001, -5.36972E-003, -6.70503E-002, 6.43438E-003, 1.06491E-001, -7.10997E-003, -5.25549E-002, 7.79181E-003, 1.06705E-001
		};

		optics_set = true;
	}

	// check
	if (!optics_set)
		throw "Optics version not understood";

	// decompose optics uncertainties
	opt_cov.SetMatrixArray(opt_cov_data.data());

	TMatrixDSymEigen eig_decomp(opt_cov);
	TVectorD eig_values(eig_decomp.GetEigenValues());
	TMatrixDSym S(n_optical_functions);
	for (unsigned int i = 0; i < n_optical_functions; i++)
		S(i, i) = (eig_values(i) >= 0.) ? sqrt(eig_values(i)) : 0.;
	opt_per_gen = eig_decomp.GetEigenVectors() * S;
}

//----------------------------------------------------------------------------------------------------

void Environment::Load(const edm::ParameterSet &ps)
{
	optics_version = ps.getParameter<string>("optics_version");

	Init();
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
	printf("optics:\n");
	printf("version = %s\n", optics_version.c_str());
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
	TVectorD r(n_optical_functions);
	for (unsigned int i = 0; i < n_optical_functions; i++)
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
