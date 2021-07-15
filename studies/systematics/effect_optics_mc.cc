#include "classes/common_algorithms.hh"
#include "classes/common_event.hh"
#include "classes/common_init.hh"
#include "classes/Kinematics.hh"
#include "classes/Stat.hh"

#include "TRandom3.h"
#include "TMatrixDSymEigen.h"

//----------------------------------------------------------------------------------------------------

int main()
{
	// run initialisation
	if (Init("config.py", "45b_56t") != 0)
		return 1;

	const unsigned int n_events = 1000;

	// central point
	const double th_x_cen = 200E-6;
	const double th_y_cen = 100E-6;

	Kinematics k_0;
	k_0.th_x_L = k_0.th_x_R = k_0.th_x = th_x_cen;
	k_0.th_y_L = k_0.th_y_R = k_0.th_y = th_y_cen;

	HitData h_0 = ProtonTransport(k_0, env);

	Stat st(4);

	Stat st_L_x_L_1_F(1), st_L_x_R_1_F(1);
	Stat st_L_y_L_1_F(1), st_L_y_R_1_F(1);

	for (unsigned int ev = 0; ev < n_events; ev++)
	{
		// generate biased optics
		Environment env_biased = env;
		env_biased.ApplyRandomOpticsPerturbations();

		// fill control stats
		st_L_x_L_1_F.Fill(env_biased.L_x_L_1_F - env.L_x_L_1_F);
		st_L_x_R_1_F.Fill(env_biased.L_x_R_1_F - env.L_x_R_1_F);
		st_L_y_L_1_F.Fill(env_biased.L_y_L_1_F - env.L_y_L_1_F);
		st_L_y_R_1_F.Fill(env_biased.L_y_R_1_F - env.L_y_R_1_F);

		// run reconstruction with biased optics
		Kinematics k = DoReconstruction(h_0, env_biased);

		// fill stats
		vector<double> deltas = {
			k.th_x_L / k_0.th_x_L - 1.,
			k.th_x_R / k_0.th_x_R - 1.,
			k.th_y_L / k_0.th_y_L - 1.,
			k.th_y_R / k_0.th_y_R - 1.
		};
		st.Fill(deltas);
	}

	// print control results
	printf("\n");
	printf("* check of input\n");
	printf("    RMS[L_x_L_1_F] = %.3f m\n", st_L_x_L_1_F.GetStdDev(0));
	printf("    RMS[L_x_R_1_F] = %.3f m\n", st_L_x_R_1_F.GetStdDev(0));
	printf("    RMS[L_y_L_1_F] = %.3f m\n", st_L_y_L_1_F.GetStdDev(0));
	printf("    RMS[L_y_R_1_F] = %.3f m\n", st_L_y_R_1_F.GetStdDev(0));

	//st.PrintMeanAndStdDev();
	//st.PrintCovariance();

	// print results
	printf("\n");
	printf("* output\n");
	printf("    RMS[de_x_L] = %.2E\n", st.GetStdDev(0));
	printf("    RMS[de_x_R] = %.2E\n", st.GetStdDev(1));
	printf("    RMS[de_y_L] = %.2E\n", st.GetStdDev(2));
	printf("    RMS[de_y_R] = %.2E\n", st.GetStdDev(3));

	// build modes
	TMatrixDSym V(4);
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
			V(i, j) = st.GetCovariance(i, j);
	}

	TMatrixDSymEigen eig_decomp(V);
	TVectorD eig_values(eig_decomp.GetEigenValues());
	TMatrixDSym S(4);
	for (unsigned int i = 0; i < 4; i++)
		S(i, i) = (eig_values(i) >= 0.) ? sqrt(eig_values(i)) : 0.;

	TMatrixD m_gen = eig_decomp.GetEigenVectors() * S;

	printf("\n");
	printf("* eigenvalues:\n");
	for (int m = 0; m < 4; ++m)
		printf("    %.2E\n", eig_values(m));

	printf("\n");
	printf("* modes (de_x_L, de_x_R, de_y_L, de_y_R):\n");
	for (int m = 0; m < 4; ++m)
	{
		printf("    mode %i: ", m);

		for (int j = 0; j < 4; ++j)
			printf("%+.2E, ", m_gen(j, m));

		printf("\n");
	}

	printf("\n");
	printf("* modes in C++ code:\n");
	for (int m = 0; m < 4; ++m)
		printf("if (mode == \"%i\") { val_L_x = %+.3E; val_R_x = %+.3E; val_L_y = %+.3E, val_R_y = %+.3E; }\n",
			m+1, m_gen(0, m), m_gen(1, m), m_gen(2, m),m_gen(3, m));
}