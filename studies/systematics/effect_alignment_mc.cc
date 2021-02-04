#include <TVectorDfwd.h>
#include <gsl/gsl_machine.h>
#include <cstdio>
#include "classes/common_algorithms.hh"
#include "classes/common_event.hh"
#include "classes/common_init.hh"
#include "classes/Kinematics.hh"
#include "classes/Stat.hh"

#include "TRandom3.h"
#include "TMatrixDSymEigen.h"

//----------------------------------------------------------------------------------------------------

TMatrixD GetGenMatrix(double si, double rho)
{
	TMatrixDSym V(2);
	V(0, 0) = si*si;
	V(0, 1) = V(1, 0) = si*si*rho;
	V(1, 1) = si*si;

	TMatrixDSymEigen eig_decomp(V);
	TVectorD eig_values(eig_decomp.GetEigenValues());
	TMatrixDSym S(2);
	for (unsigned int i = 0; i < 2; i++)
		S(i, i) = (eig_values(i) >= 0.) ? sqrt(eig_values(i)) : 0.;

	return eig_decomp.GetEigenVectors() * S;
}

//----------------------------------------------------------------------------------------------------

TVectorD GetRandomVector(const TMatrixD &m_gen)
{
	TVectorD r(m_gen.GetNcols());
	for (int i = 0; i < m_gen.GetNcols(); ++i)
		r(i) = gRandom->Gaus();

	return m_gen * r;
}

//----------------------------------------------------------------------------------------------------

int main()
{
	// run initialisation
	if (Init("config.py", "45b_56t") != 0)
		return 1;

	// uncertainties
	const double x_sh_unc = 100E-3;		// mm
	const double x_sh_NF_unc = 10E-3;	// mm
	const double x_sh_NF_rho = 1. - x_sh_NF_unc * x_sh_NF_unc / 2. / x_sh_unc / x_sh_unc;

	const double y_sh_unc = 250E-3;		// mm
	const double y_sh_NF_rho = 0.;

	const double tilt_unc = 5E-3;		// rad
	const double tilt_NF_unc = 0.5E-3;	// rad
	const double tilt_NF_rho = 1. - tilt_NF_unc * tilt_NF_unc / 2. / tilt_unc / tilt_unc;

	// uncertainty matrices
	const TMatrixD &m_gen_sh_x = GetGenMatrix(x_sh_unc, x_sh_NF_rho);
	const TMatrixD &m_gen_sh_y = GetGenMatrix(y_sh_unc, y_sh_NF_rho);
	const TMatrixD &m_gen_tilt = GetGenMatrix(tilt_unc, tilt_NF_rho);
	
	// central point
	const double th_x_cen = 200E-6;
	const double th_y_cen = 100E-6;

	Kinematics k_0;
	k_0.th_x_L = k_0.th_x_R = k_0.th_x = th_x_cen;
	k_0.th_y_L = k_0.th_y_R = k_0.th_y = th_y_cen;

	HitData h_0 = ProtonTransport(k_0, env);

	Stat st_de_x(1), st_de_x_NF(1);
	Stat st_de_y(1), st_de_y_NF(1);

	Stat st_de_th_x_L(1), st_de_th_x_R(1), st_de_th_x_G(1);
	Stat st_de_th_y_L(1), st_de_th_y_R(1), st_de_th_y_G(1);

	Stat st_tilt(1), st_tilt_NF(1);

	// effect of tilt
	//	th*_x --> th*_x + C * th*_y
	//	th*_y --> th*_y + D * th*_x
	Stat st_C_L(1), st_C_R(1), st_C_G(1);
	Stat st_D_L(1), st_D_R(1), st_D_G(1);

	for (unsigned int n_ev = 0; n_ev < 10000; n_ev++)
	{
		// generate shift perturbations
		const TVectorD &de_x_L = GetRandomVector(m_gen_sh_x);
		const TVectorD &de_x_R = GetRandomVector(m_gen_sh_x);

		const TVectorD &de_y_L = GetRandomVector(m_gen_sh_y);
		const TVectorD &de_y_R = GetRandomVector(m_gen_sh_y);

		// run reconstruction
		HitData h = h_0;
		h.L_1_F.x += de_x_L(0); h.L_1_F.y += de_y_L(0);
		h.L_2_F.x += de_x_L(1); h.L_2_F.y += de_y_L(1);

		h.R_1_F.x += de_x_R(0); h.R_1_F.y += de_y_R(0);
		h.R_2_F.x += de_x_R(1); h.R_2_F.y += de_y_R(1);

		Kinematics k = DoReconstruction(h, env);

		// fill stats
		st_de_x.Fill(de_x_L(0));
		st_de_x_NF.Fill(de_x_L(1) - de_x_L(0));

		st_de_y.Fill(de_y_L(0));
		st_de_y_NF.Fill(de_y_L(1) - de_y_L(0));

		st_de_th_x_L.Fill(k.th_x_L - k_0.th_x_L);
		st_de_th_x_R.Fill(k.th_x_R - k_0.th_x_R);
		st_de_th_x_G.Fill(k.th_x - k_0.th_x);

		st_de_th_y_L.Fill(k.th_y_L - k_0.th_y_L);
		st_de_th_y_R.Fill(k.th_y_R - k_0.th_y_R);
		st_de_th_y_G.Fill(k.th_y - k_0.th_y);

		// generate rot perturbations
		const TVectorD &tilt_L = GetRandomVector(m_gen_tilt);
		const TVectorD &tilt_R = GetRandomVector(m_gen_tilt);

		// run reco after tilt
		HitData h_tilt = h_0;
		h_tilt.L_1_F.x += +tilt_L(0) * h_0.L_1_F.y; h_tilt.L_1_F.y += -tilt_L(0) * h_0.L_1_F.x;
		h_tilt.L_2_F.x += +tilt_L(1) * h_0.L_2_F.y; h_tilt.L_2_F.y += -tilt_L(1) * h_0.L_2_F.x;

		h_tilt.R_1_F.x += +tilt_R(0) * h_0.R_1_F.y; h_tilt.R_1_F.y += -tilt_R(0) * h_0.R_1_F.x;
		h_tilt.R_2_F.x += +tilt_R(1) * h_0.R_2_F.y; h_tilt.R_2_F.y += -tilt_R(1) * h_0.R_2_F.x;

		Kinematics k_tilt = DoReconstruction(h_tilt, env);

		// fill stats
		st_tilt.Fill(tilt_L(0));
		st_tilt_NF.Fill(tilt_L(1) - tilt_L(0));

		st_C_L.Fill( (k_tilt.th_x_L - k_0.th_x_L) / th_y_cen );
		st_C_R.Fill( (k_tilt.th_x_R - k_0.th_x_R) / th_y_cen );
		st_C_G.Fill( (k_tilt.th_x - k_0.th_x) / th_y_cen );

		st_D_L.Fill( (k_tilt.th_y_L - k_0.th_y_L) / th_x_cen );
		st_D_R.Fill( (k_tilt.th_y_R - k_0.th_y_R) / th_x_cen );
		st_D_G.Fill( (k_tilt.th_y - k_0.th_y) / th_x_cen );
	}

	printf("* verification:\n");
	printf("    RMS[de_x] = %.3f\n", st_de_x.GetStdDev(0));
	printf("    RMS[de_x_F - de_x_N] = %.3f\n", st_de_x_NF.GetStdDev(0));
	printf("    RMS[de_y] = %.3f\n", st_de_y.GetStdDev(0));
	printf("    RMS[de_y_F - de_y_N] = %.3f\n", st_de_y_NF.GetStdDev(0));
	printf("    RMS[tilt] = %.2E\n", st_tilt.GetStdDev(0));
	printf("    RMS[tilt_F - tilt_N] = %.2E\n", st_tilt_NF.GetStdDev(0));

	printf("\n");
	printf("* shifts:\n");
	printf("    RMS[de_th_x_L] = %.1f urad\n", st_de_th_x_L.GetStdDev(0) * 1E6);
	printf("    RMS[de_th_x_R] = %.1f urad\n", st_de_th_x_R.GetStdDev(0) * 1E6);
	printf("    RMS[de_th_x_G] = %.1f urad\n", st_de_th_x_G.GetStdDev(0) * 1E6);
	printf("\n");
	printf("    RMS[de_th_y_L] = %.1f urad\n", st_de_th_y_L.GetStdDev(0) * 1E6);
	printf("    RMS[de_th_y_R] = %.1f urad\n", st_de_th_y_R.GetStdDev(0) * 1E6);
	printf("    RMS[de_th_y_G] = %.1f urad\n", st_de_th_y_G.GetStdDev(0) * 1E6);

	printf("\n");
	printf("* tilts:\n");
	printf("    RMS[C_L] = %.2E\n", st_C_L.GetStdDev(0) * 1E0);
	printf("    RMS[C_R] = %.2E\n", st_C_R.GetStdDev(0) * 1E0);
	printf("    RMS[C_G] = %.2E\n", st_C_G.GetStdDev(0) * 1E0);

	printf("    RMS[D_L] = %.2E\n", st_D_L.GetStdDev(0) * 1E0);
	printf("    RMS[D_R] = %.2E\n", st_D_R.GetStdDev(0) * 1E0);
	printf("    RMS[D_G] = %.2E\n", st_D_G.GetStdDev(0) * 1E0);
}
